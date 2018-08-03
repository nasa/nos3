 /*************************************************************************
 ** File:
 **   $Id: sc_state.c 1.10 2015/03/02 12:59:13EST sstrege Exp  $
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
 **   This file contains functions to handle getting the next time of
 **   commands for the ATP and RTP  as well as updating the time for
 **   Stored Command.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sc_state.c  $
 **   Revision 1.10 2015/03/02 12:59:13EST sstrege 
 **   Added copyright information
 **   Revision 1.9 2014/06/06 11:37:59EDT sjudy 
 **   Changed event msgs to have 'RTS' or 'ATS' instead of "Real Time Sequence", etc.
 **   Revision 1.8 2010/09/28 06:33:10GMT-08:00 lwalling 
 **   Update list of included header files
 **   Revision 1.7 2010/05/18 15:30:39EDT lwalling 
 **   Change AtsId/RtsId to AtsIndex/RtsIndex or AtsNumber/RtsNumber
 **   Revision 1.6 2010/05/18 14:13:12EDT lwalling 
 **   Change AtsCmdIndexBuffer contents from entry pointer to entry index
 **   Revision 1.5 2010/03/26 18:03:32EDT lwalling 
 **   Remove pad from ATS and RTS structures, change 32 bit ATS time to two 16 bit values
 **   Revision 1.4 2010/03/26 11:28:07EDT lwalling 
 **   Fixed packet length calculation to support odd byte length ATS and RTS commands
 **   Revision 1.3 2009/01/26 14:47:15EST nyanchik 
 **   Check in of Unit test
 **   Revision 1.2 2009/01/05 08:26:57EST nyanchik 
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
#include "sc_rtsrq.h"
#include "sc_state.h"
#include "sc_utils.h"
#include "sc_events.h"
#include "sc_msgdefs.h"
#include "sc_tbldefs.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  Gets the time of the next RTS command                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_GetNextRtsTime (void)
{
    
    int16               i;          /* loop counter MUST be SIGNED !*/
    uint16              NextRts;    /* the next rts to schedule */
    SC_AbsTimeTag_t     NextTime;   /* the next time for the RTS */

    NextRts = 0xFFFF;
    NextTime = SC_MAX_TIME;
        
    /*
     ** Go through the table backwards to account for the RTS priority 
     ** Lower number RTS's get higher priority
     */
    for (i = SC_NUMBER_OF_RTS - 1; i >= 0; i--)
    {
        if (SC_OperData.RtsInfoTblAddr[i].RtsStatus == SC_EXECUTING)
        {   
            if (SC_OperData.RtsInfoTblAddr[i].NextCommandTime <= NextTime)
            {       
                NextTime = SC_OperData.RtsInfoTblAddr[i].NextCommandTime;
                NextRts = i;   
            } /* end if */ 
        } /* end if */ 
    } /* end for */
    
    if (NextRts == 0xFFFF)
    {
        SC_OperData.RtsCtrlBlckAddr -> RtsNumber = SC_INVALID_RTS_NUMBER;
        SC_AppData.NextCmdTime[SC_RTP] = SC_MAX_TIME;
    }
    else
    {
        SC_OperData.RtsCtrlBlckAddr -> RtsNumber = NextRts + 1;
        SC_AppData.NextCmdTime[SC_RTP] = NextTime;
    } /* end if */
    
    
} /* end SC_GetNextRtsTime */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Decides whether an RTS or ATS command gets scheduled next       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_UpdateNextTime (void)
{
    /*
     ** First, find out which RTS needs to run next
     */
    SC_GetNextRtsTime();
    
    /*
     ** Start out with a default, no processors need to run next
     */
    SC_AppData.NextProcNumber = SC_NONE;
    
    
    /*
     ** Check to see if the ATP needs to schedule commands
     */
    if (SC_OperData.AtsCtrlBlckAddr -> AtpState == SC_EXECUTING)
    {
        SC_AppData.NextProcNumber = SC_ATP;
    }
    /*
     ** Last, check to see if there is an RTS that needs to schedule commands
     ** This is determined by the RTS number in the RTP control block
     ** If it is zero, there is no RTS that needs to run
     */
    if (SC_OperData.RtsCtrlBlckAddr -> RtsNumber > 0 &&
        SC_OperData.RtsCtrlBlckAddr -> RtsNumber <= SC_NUMBER_OF_RTS)
    { 
        /*
         ** If the RTP needs to send commands, only send them if
         ** the RTP time is less than the ATP time. Otherwise
         ** the ATP has priority
         */
        if (SC_AppData.NextCmdTime[SC_RTP] < SC_AppData.NextCmdTime[SC_ATP])
        {
            SC_AppData.NextProcNumber = SC_RTP;
        }       
    } /* end if */
} /* end SC_UpdateNextTime */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Gets the next RTS Command                                       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_GetNextRtsCommand(void)
{ 
    uint16                      RtsNum;
    uint16                      CmdOffset;
    CFE_SB_MsgPtr_t             CmdPtr;
    SC_RtsEntryHeader_t        *RtsEntryPtr;
    uint16                      CmdLength;
       
    /*
     ** Make sure that the RTP is executing some RTS
     */
     
    if ((SC_OperData.RtsCtrlBlckAddr -> RtsNumber > 0) &&
         (SC_OperData.RtsCtrlBlckAddr -> RtsNumber <= SC_NUMBER_OF_RTS))
    {
        /* Get the number of the rts that is running */
        RtsNum = SC_OperData.RtsCtrlBlckAddr -> RtsNumber - 1;
        /*
         ** Find out if the RTS is EXECUTING or just STARTED
         */
        if (SC_OperData.RtsInfoTblAddr[RtsNum].RtsStatus == SC_EXECUTING)
        {
            /*
             ** Get the information needed to find the next command
             */
            CmdOffset = SC_OperData.RtsInfoTblAddr[RtsNum].NextCommandPtr;
                 
            
            RtsEntryPtr = (SC_RtsEntryHeader_t *) &SC_OperData.RtsTblAddr[RtsNum][CmdOffset];
            CmdPtr = (CFE_SB_MsgPtr_t) RtsEntryPtr->CmdHeader;
            
            CmdLength = CFE_SB_GetTotalMsgLength(CmdPtr) + SC_RTS_HEADER_SIZE;
         
            
            /*
             ** calculate the new command offset and new command length
             ** Cmd Length is in bytes, so we convert it to words
             ** (plus 1 to round byte len up to word len)
             */
            CmdOffset = CmdOffset + ((CmdLength + 1) / SC_BYTES_IN_WORD);

            /*
             ** if the end of the buffer is not reached.
             ** This check is made to make sure that at least the minimum
             ** Sized packet fits in the buffer. It assures we are not reading
             ** bogus length info from other data.
             */

            /* If at least the header for a command plus the RTS header can fit in the buffer */
            if (CmdOffset <  (SC_RTS_BUFF_SIZE - (sizeof(SC_RtsEntryHeader_t) / SC_BYTES_IN_WORD)))
            {
                /*
                 ** Get a pointer to the next RTS command
                 */
                RtsEntryPtr = (SC_RtsEntryHeader_t *) &SC_OperData.RtsTblAddr[RtsNum][CmdOffset];
                CmdPtr = (CFE_SB_MsgPtr_t) RtsEntryPtr->CmdHeader;
                
                /*
                 ** get the length of the new command
                 */
                CmdLength = CFE_SB_GetTotalMsgLength(CmdPtr) + SC_RTS_HEADER_SIZE;

                /*
                 ** Check to see if the command length is less than the size of a header.
                 ** This indicates that there are no more commands
                 */
                
                if ((CmdLength - SC_RTS_HEADER_SIZE) >= (SC_PACKET_MIN_SIZE))
                {
                    /*
                     ** Check to see if the command length is too big
                     ** If it is , then there is an error with the command
                     */
                    if ((CmdLength - SC_RTS_HEADER_SIZE) <= SC_PACKET_MAX_SIZE )
                    { 
                        /*
                         ** Last Check is to check to see if the command
                         ** runs off of the end of the buffer
                         ** (plus 1 to round byte len up to word len)
                         */
                        if (CmdOffset + ((CmdLength + 1) / SC_BYTES_IN_WORD) <= SC_RTS_BUFF_SIZE)
                        {
                            /*
                             ** Everything passed!
                             ** Update the proper next command time for that RTS
                             */
                            SC_OperData.RtsInfoTblAddr[RtsNum].NextCommandTime = 
                            SC_ComputeAbsTime(RtsEntryPtr->TimeTag);
                            
                            /*
                             ** Update the appropriate RTS info table current command pointer
                             */
                            SC_OperData.RtsInfoTblAddr[RtsNum].NextCommandPtr = CmdOffset;
                            
                        }
                        else
                        { /* the command runs past the end of the buffer */
                            
                            /*
                             ** Having a command that runs off of the end of the buffer
                             ** is an error condition, so record it
                             */
                            SC_AppData.RtsCmdErrCtr++;
                            SC_OperData.RtsInfoTblAddr[RtsNum].CmdErrCtr++;
                            SC_AppData.LastRtsErrSeq = SC_OperData.RtsCtrlBlckAddr -> RtsNumber;
                            SC_AppData.LastRtsErrCmd = CmdOffset;
                            
                            /*
                             ** Stop the RTS from executing
                             */
                            SC_KillRts (RtsNum);
                            CFE_EVS_SendEvent (SC_RTS_LNGTH_ERR_EID,
                                               CFE_EVS_ERROR,
                                               "Cmd Runs passed end of table, RTS %03d Aborted",
                                               SC_OperData.RtsCtrlBlckAddr -> RtsNumber);
                            
                        } /* end if the command runs off the end of the buffer */
                        
                    }
                    else
                    { /* the command length is too large */

                        /* update the error information */
                        SC_AppData.RtsCmdErrCtr++;
                        SC_OperData.RtsInfoTblAddr[RtsNum].CmdErrCtr++;
                        SC_AppData.LastRtsErrSeq = SC_OperData.RtsCtrlBlckAddr -> RtsNumber;
                        SC_AppData.LastRtsErrCmd = CmdOffset;
                        
                        /* Stop the RTS from executing */
                        SC_KillRts (RtsNum);
                        CFE_EVS_SendEvent (SC_RTS_CMD_LNGTH_ERR_EID,
                                           CFE_EVS_ERROR,
                                           "Invalid Length Field in RTS Command, RTS %03d Aborted. Length: %d, Max: %d",
                                           SC_OperData.RtsCtrlBlckAddr -> RtsNumber,
                                           (CmdLength - SC_RTS_HEADER_SIZE),
                                           SC_PACKET_MAX_SIZE);
                        
                    } /* end if the command length is invalid */
                }
                else
                { /* The command length is zero indicating no more cmds */
                    
                    /*
                     **  This is not an error condition, so stop the RTS
                     */

                    /* Stop the RTS from executing */
                    SC_KillRts (RtsNum);
                    if ((RtsNum + 1) <= SC_LAST_RTS_WITH_EVENTS)
                    {
                        CFE_EVS_SendEvent (SC_RTS_COMPL_INF_EID,
                                           CFE_EVS_INFORMATION,
                                           "RTS %03d Execution Completed",
                                           RtsNum + 1);
                    }
                }
            }
            else
            {  /* The end of the RTS buffer has been reached... */
                
                /* Stop the RTS from executing */
                SC_KillRts (RtsNum);
                if ((RtsNum + 1) <= SC_LAST_RTS_WITH_EVENTS)
                {
                    CFE_EVS_SendEvent (SC_RTS_COMPL_INF_EID,
                                       CFE_EVS_INFORMATION,
                                       "RTS %03d Execution Completed",
                                       RtsNum + 1);
                }
                
            } /* end if */
            
        } /* end if the RTS status is EXECUTING */
        
    } /* end if the RTS number is valid */
    
} /* end SC_GetNextRtsCommand */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Gets the next ATS Command                                       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_GetNextAtsCommand(void)
{
    
    uint16      AtsIndex;         /* ats array index */
    uint16      TimeIndex;     /* a time index pointer */

    
    if (SC_OperData.AtsCtrlBlckAddr -> AtpState == SC_EXECUTING)
    {
        
        /*
         ** Get the information that is needed to find the next command
         */
        AtsIndex = SC_OperData.AtsCtrlBlckAddr -> AtsNumber - 1;
        TimeIndex = SC_OperData.AtsCtrlBlckAddr -> TimeIndexPtr + 1;
        
        /*
         ** Check to see if there are more ATS commands
         */
        if (TimeIndex < SC_OperData.AtsInfoTblAddr[AtsIndex].NumberOfCommands)
        {

            
            /* get the information for the next command in the ATP control block */
            SC_OperData.AtsCtrlBlckAddr -> TimeIndexPtr = TimeIndex;
            SC_OperData.AtsCtrlBlckAddr -> CmdNumber = SC_AppData.AtsTimeIndexBuffer[AtsIndex][TimeIndex];
            
            /* update the next command time */
            SC_AppData.NextCmdTime[SC_ATP] = SC_GetAtsEntryTime((SC_AtsEntryHeader_t *)
               &SC_OperData.AtsTblAddr[AtsIndex][SC_AppData.AtsCmdIndexBuffer[AtsIndex][SC_OperData.AtsCtrlBlckAddr->CmdNumber]]);
        }
        else
        { /* the end is near... of the ATS buffer that is */
            
            /* stop the ATS */
            SC_KillAts();
            CFE_EVS_SendEvent (SC_ATS_COMPL_INF_EID,
                               CFE_EVS_INFORMATION,
                               "ATS %c Execution Completed",
                               (AtsIndex ? 'B' : 'A'));

            
            /* stop any switch that is pending */
            /* because we just ran out of commands and are stopping the ATS */
            /* and for the safe switch pend, that is a no-no */
            SC_OperData.AtsCtrlBlckAddr -> SwitchPendFlag = FALSE;
            
        } /* end if */
        
    }
    else if (SC_OperData.AtsCtrlBlckAddr -> AtpState == SC_STARTING)
    {        
        SC_OperData.AtsCtrlBlckAddr -> AtpState = SC_EXECUTING;
        
    } /* end if ATS is EXECUTING*/
    
} /* end SC_GetNextAtsCommand */


/************************/
/*  End of File Comment */
/************************/

