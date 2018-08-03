 /*************************************************************************
 ** File:
 **   $Id: sc_atsrq.c 1.14 2015/03/02 12:58:54EST sstrege Exp  $
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
 **     This file contains functions to handle all of the ATS
 **     executive requests and internal reuqests to control
 **     the ATP and ATSs.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sc_atsrq.c  $
 **   Revision 1.14 2015/03/02 12:58:54EST sstrege 
 **   Added copyright information
 **   Revision 1.13 2014/06/06 11:37:49EDT sjudy 
 **   Changed event msgs to have 'RTS' or 'ATS' instead of "Real Time Sequence", etc.
 **   Revision 1.12 2011/01/28 13:20:15GMT-08:00 lwalling 
 **   Store ATS selection from most recent ATS Append command
 **   Revision 1.11 2010/09/28 10:34:34EDT lwalling 
 **   Update list of included header files
 **   Revision 1.10 2010/05/18 15:32:06EDT lwalling 
 **   Change AtsId/RtsId to AtsIndex/RtsIndex or AtsNumber/RtsNumber
 **   Revision 1.9 2010/05/18 14:13:59EDT lwalling 
 **   Change AtsCmdIndexBuffer contents from entry pointer to entry index
 **   Revision 1.8 2010/05/05 11:16:40EDT lwalling 
 **   Cleanup function return code definitions, create error specific event numbers
 **   Revision 1.7 2010/04/21 15:38:24EDT lwalling 
 **   Moved prototype for SC_BeginAts to header file, added cmd handler for Append ATS
 **   Revision 1.6 2010/04/15 15:19:17EDT lwalling 
 **   Fix typo - remove ampersand from arg in call to get ATS time value
 **   Revision 1.5 2010/03/26 18:02:13EDT lwalling 
 **   Remove pad from ATS and RTS structures, change 32 bit ATS time to two 16 bit values
 **   Revision 1.4 2009/01/27 08:46:01EST nyanchik 
 **   Continue SC unit test
 **   Revision 1.3 2009/01/26 14:44:42EST nyanchik 
 **   Check in of Unit test
 **   Revision 1.2 2009/01/05 08:26:50EST nyanchik 
 **   Check in after code review changes
 
 *************************************************************************/
 
 

/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/

#include "cfe.h"
#include "sc_atsrq.h"
#include "sc_loads.h"
#include "sc_utils.h"
#include "sc_events.h"

/**************************************************************************
 **
 ** Functions
 **
 **************************************************************************/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Starts an ATS                                                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_StartAtsCmd (CFE_SB_MsgPtr_t CmdPacket)
{
    uint16         AtsIndex;           /* ATS array index */

    if (SC_VerifyCmdLength(CmdPacket, sizeof(SC_StartAtsCmd_t)))
    {
        
        /* convert ATS ID to array index */
        AtsIndex = (((SC_StartAtsCmd_t*)CmdPacket) -> AtsId) - 1;
        
        /* validate ATS array index */
        if (AtsIndex < SC_NUMBER_OF_ATS)
        { 
            /* make sure that there is no ATS running on the ATP */
            if (SC_OperData.AtsCtrlBlckAddr -> AtpState == SC_IDLE)
            {       
                /* make sure the specified ATS is ready */
                if (SC_OperData.AtsInfoTblAddr[AtsIndex].NumberOfCommands > 0)
                {
                    /* start the ats */
                    if (SC_BeginAts (AtsIndex, 0))
                    {           
                        /* finish the ATP control block .. */
                        SC_OperData.AtsCtrlBlckAddr -> AtpState = SC_EXECUTING;
                        
                        /* increment the command request counter */
                        SC_AppData.CmdCtr++;
                        
                        CFE_EVS_SendEvent(SC_STARTATS_CMD_INF_EID,
                                          CFE_EVS_INFORMATION,
                                          "ATS %c Execution Started",
                                          (AtsIndex ? 'B' : 'A'));
                    }
                    else
                    {  /* could not start the ats, all commands were skipped */
                        /* event message was sent from SC_BeginAts */
                        /* increment the command request error counter */
                        SC_AppData.CmdErrCtr++;

                    }  /* end if */   
                }
                else
                {  /* the ats didn't have any commands in it */
                    
                    CFE_EVS_SendEvent(SC_STARTATS_CMD_NOT_LDED_ERR_EID,
                                      CFE_EVS_ERROR,
                                      "Start ATS Rejected: ATS %c Not Loaded",
                                      (AtsIndex ? 'B' : 'A'));
                    
                    /* increment the command request error counter */
                    SC_AppData.CmdErrCtr++;
                    
                } /* end if */
                
            }
            else
            { /* the ATS is being used */
                
                CFE_EVS_SendEvent(SC_STARTATS_CMD_NOT_IDLE_ERR_EID,
                                  CFE_EVS_ERROR,
                                  "Start ATS Rejected: ATP is not Idle");
                /* increment the command request error counter */
                SC_AppData.CmdErrCtr++;
                
            } /* end if */
        }
        else
        { /* the specified ATS id is not valid */
            
            CFE_EVS_SendEvent(SC_STARTATS_CMD_INVLD_ID_ERR_EID,
                              CFE_EVS_ERROR,
                              "Start ATS %d Rejected: Invalid ATS ID",
                              ((SC_StartAtsCmd_t*)CmdPacket) -> AtsId);
            
            /* increment the command request error counter */
            SC_AppData.CmdErrCtr++;
            
        } /* end if */ 
    }
} /* end SC_StartAtsCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*   Stop the currently executing ATS                              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_StopAtsCmd (CFE_SB_MsgPtr_t CmdPacket)
{  
    uint16  TempAtsChar = ' ';
    int32   Result  =  SC_ERROR;

    if (SC_VerifyCmdLength(CmdPacket, sizeof(SC_NoArgsCmd_t)))
    {       
        /*
         ** Set the temp ATS ID if it is valid
         */
        if (SC_OperData.AtsCtrlBlckAddr -> AtsNumber == SC_ATSA)
        {
            TempAtsChar = 'A';
            Result = CFE_SUCCESS;
        }
        else 
        {
          if (SC_OperData.AtsCtrlBlckAddr -> AtsNumber == SC_ATSB)
          {
            TempAtsChar = 'B';
            Result = CFE_SUCCESS;
          }

        }
        
        if (Result == CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(SC_STOPATS_CMD_INF_EID,
                              CFE_EVS_INFORMATION,
                              "ATS %c stopped",
                              TempAtsChar);
        }
        else
        {
            CFE_EVS_SendEvent(SC_STOPATS_NO_ATS_INF_EID,
                              CFE_EVS_INFORMATION,
                             "There is no ATS running to stop");
        }
            
        
        /* Stop the ATS from executing */
        SC_KillAts();
        
        /* clear the global switch pend flag */
        SC_OperData.AtsCtrlBlckAddr -> SwitchPendFlag = FALSE;
        
        SC_AppData.CmdCtr++;
        
    }
    
} /* end SC_StopAtsCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Function for stating an ATS                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean SC_BeginAts (uint16 AtsIndex, uint16 TimeOffset)
{
    SC_AtsEntryHeader_t   *Entry;         /* ATS table entry pointer */
    int32                  EntryIndex;    /* ATS entry location in table */
    SC_AbsTimeTag_t        ListCmdTime;   /* list entry execution time */
    int32                  TimeIndex;     /* the current time buffer index */
    int32                  CmdIndex;      /* ATS command index (cmd num - 1) */
    boolean                ReturnCode;
    SC_AbsTimeTag_t        TimeToStartAts;    /* the REAL time to start the ATS */
    uint16                 CmdsSkipped = 0;
    
    TimeToStartAts = SC_ComputeAbsTime (TimeOffset);
    
    /*
     ** Loop through the commands until a time tag is found that
     ** has a time greater than or equal to the current time OR
     ** all of the commands have been skipped
     */
    TimeIndex = 0;   /* pointer into the time index table */
    
    while (TimeIndex < SC_OperData.AtsInfoTblAddr[AtsIndex].NumberOfCommands)
    {
        /* first get the cmd index at this list entry */
        CmdIndex = SC_AppData.AtsTimeIndexBuffer[AtsIndex][TimeIndex];
        /* then get the entry index from the cmd index table */
        EntryIndex = SC_AppData.AtsCmdIndexBuffer[AtsIndex][CmdIndex];
        /* then get a pointer to the ATS entry data */
        Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[AtsIndex][EntryIndex];
        /* then get cmd execution time from the ATS entry */
        ListCmdTime = SC_GetAtsEntryTime(Entry);

        /* compare ATS start time to this list entry time */
        if (SC_CompareAbsTime(TimeToStartAts, ListCmdTime))
        {
            /* start time is greater than this list entry time */

            SC_OperData.AtsCmdStatusTblAddr[AtsIndex][CmdIndex] = SC_SKIPPED;
            CmdsSkipped++;
            TimeIndex++;
        }
        else
        {
            /* start time is less than or equal to this list entry */
            break;
        }
    }
    
    /*
     ** Check to see if the whole ATS was skipped
     */
    if (TimeIndex == SC_OperData.AtsInfoTblAddr[AtsIndex].NumberOfCommands)
    {
        
        CFE_EVS_SendEvent(SC_ATS_SKP_ALL_ERR_EID,
                          CFE_EVS_ERROR,
                          "All ATS commands were skipped, ATS stopped");
        
        /* stop the ats */
        SC_KillAts();
        
        ReturnCode = FALSE;
        
    }
    else
    {  /* there is at least one command to execute */
        
        /*
         ** Initialize the ATP Control Block.
         */
        /* leave the atp state alone, it will be updated by the caller */
        SC_OperData.AtsCtrlBlckAddr -> AtsNumber = AtsIndex + 1;
        SC_OperData.AtsCtrlBlckAddr -> CmdNumber = CmdIndex;
        SC_OperData.AtsCtrlBlckAddr -> TimeIndexPtr = TimeIndex;

        /* send an event for number of commands skipped */
        CFE_EVS_SendEvent(SC_ATS_ERR_SKP_DBG_EID,
                          CFE_EVS_DEBUG,
                          "ATS started, skipped %d commands",
                          CmdsSkipped);
        /*
         ** Set the next command time for the ATP
         */
        SC_AppData.NextCmdTime[SC_ATP] = ListCmdTime;

        ReturnCode = TRUE;
        
    }  /* end if */
    
    return (ReturnCode);
    
} /* end SC_BeginAts */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  Function for stopping the running ATS  & clearing data         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_KillAts (void)
{

    
    if (SC_OperData.AtsCtrlBlckAddr -> AtpState !=  SC_IDLE)
    {
        /* Increment the ats use counter */
        SC_OperData.AtsInfoTblAddr[SC_OperData.AtsCtrlBlckAddr -> AtsNumber - 1].AtsUseCtr++;
    }
    /*
     ** Reset the state in the atp control block
     */
    SC_OperData.AtsCtrlBlckAddr -> AtpState = SC_IDLE;
    
    /* reset the time of the next ats command */
    SC_AppData.NextCmdTime[SC_ATP] = SC_MAX_TIME;
    
} /* end SC_KillAts */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Process an ATS Switch                                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_GroundSwitchCmd (CFE_SB_MsgPtr_t CmdPacket)
{
    
    uint16         CurrAtsNum;
    uint16         NewAtsNum;      /* the number of the ats to switch to*/

    if (SC_VerifyCmdLength(CmdPacket, sizeof(SC_NoArgsCmd_t)))
    {
        /* make sure that an ATS is running on the ATP */
        if (SC_OperData.AtsCtrlBlckAddr -> AtpState == SC_EXECUTING)
        {
            /* get the current ATS number range 0..1 */
            CurrAtsNum = SC_OperData.AtsCtrlBlckAddr -> AtsNumber - 1;
            
            /* get the ATS to switch to */
            NewAtsNum = 1 - CurrAtsNum;
            
            /* Now check to see if the new ATS has commands in it */
            if (SC_OperData.AtsInfoTblAddr[NewAtsNum].NumberOfCommands > 0)
            {
                
                /* set the global switch pend flag */
                SC_OperData.AtsCtrlBlckAddr -> SwitchPendFlag = TRUE;
                
                /* update the command counter */
                SC_AppData.CmdCtr++;
                
                CFE_EVS_SendEvent(SC_SWITCH_ATS_CMD_INF_EID,
                                  CFE_EVS_INFORMATION,
                                  "Switch ATS is Pending");  
            }
            else
            { /* the other ATS does not have any commands in it */     
                
                CFE_EVS_SendEvent(SC_SWITCH_ATS_CMD_NOT_LDED_ERR_EID,
                                  CFE_EVS_ERROR,
                                  "Switch ATS Failure: Destination ATS Not Loaded");
                
                /* update command error counter */
                SC_AppData.CmdErrCtr++;
                
                SC_OperData.AtsCtrlBlckAddr -> SwitchPendFlag = FALSE;
                
            } /* end if */           
        }
        else
        {  /* the ATP is not currently executing any commands */
            
            CFE_EVS_SendEvent(SC_SWITCH_ATS_CMD_IDLE_ERR_EID,
                              CFE_EVS_ERROR,
                              "Switch ATS Rejected: ATP is idle");
            
            /* update the command error counter */
            SC_AppData.CmdErrCtr++;
            
            SC_OperData.AtsCtrlBlckAddr -> SwitchPendFlag = FALSE;
            
        } /* end if */
    }  
} /* end SC_GroundSwitchCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Function for switching ATS's when each have commands in to      */
/* execute in the same second.                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_ServiceSwitchPend (void)
{
    uint16    NewAtsNum;    /* the ats that we are switching to */
    uint16    OldAtsNum;    /* the ats we are switching from */

    /*
     **  See if it is time to switch the ATS
     */
    if (SC_CompareAbsTime (SC_AppData.NextCmdTime[SC_ATP], SC_AppData.CurrentTime))
    {
        
        /* make sure that an ATS is still running on the ATP */
        if (SC_OperData.AtsCtrlBlckAddr -> AtpState == SC_EXECUTING)
        {
            
            /* get the ATS number to switch to and from */
            OldAtsNum = SC_OperData.AtsCtrlBlckAddr -> AtsNumber - 1;
            NewAtsNum = 1 - (SC_OperData.AtsCtrlBlckAddr -> AtsNumber - 1);
            
            /* Now check to see if the new ATS has commands in it */
            if (SC_OperData.AtsInfoTblAddr[NewAtsNum].NumberOfCommands > 0)
            {
                
                /* stop the current ATS */
                SC_KillAts();
                
                /*
                 ** Start the new ATS: Notice that we are starting the new
                 ** ATS with a one second offset from the current second,
                 ** This prevents commands that were executed the same
                 ** second that this command was received from being repeated.
                 */
                if (SC_BeginAts (NewAtsNum, 1))
                {
                    
                    SC_OperData.AtsCtrlBlckAddr -> AtpState = SC_EXECUTING;
                    
                    CFE_EVS_SendEvent(SC_ATS_SERVICE_SWTCH_INF_EID,
                                      CFE_EVS_INFORMATION ,
                                      "ATS Switched from %c to %c",
                                      (OldAtsNum?'B':'A'), (NewAtsNum?'B':'A'));
                    
                }  /* end if */
            }
            else
            { /* the other ATS does not have any commands in it */
                
                CFE_EVS_SendEvent(SC_SERVICE_SWITCH_ATS_CMD_LDED_ERR_EID,
                                  CFE_EVS_ERROR,
                                  "Switch ATS Failure: Destination ATS is empty");   
            } /* end if */
        }
        else
        {   /* the ATP is not currently executing any commands */
            /* this should only happen if the switch flag gets */
            /* corrupted some how                              */
            
            CFE_EVS_SendEvent(SC_ATS_SERVICE_SWITCH_IDLE_ERR_EID,
                              CFE_EVS_ERROR ,
                              "Switch ATS Rejected: ATP is idle");            
        } /* end if */
        
        /* in any case, this flag will need to be cleared */
        SC_OperData.AtsCtrlBlckAddr -> SwitchPendFlag = FALSE;
        
    } /* end if */
    
} /* end SC_ServiceSwitchPend */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Switches from one ATS to the other when there are no commands   */
/* to be executed in the same second of the switch                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean SC_InlineSwitch (void)
{

    uint16         NewAtsNum;      /* the number of the ats to switch to*/
    uint16         OldAtsNum;      /* the number of the ats to switch from*/
    boolean        ReturnCode;      /* return code for function */

    
    /* figure out which ATS to switch to */
    NewAtsNum = 1 - (SC_OperData.AtsCtrlBlckAddr -> AtsNumber - 1);
    
    /* Save the ATS number to switch FROM */
    OldAtsNum = SC_OperData.AtsCtrlBlckAddr -> AtsNumber - 1;

    
    /* Now check to see if the new ATS has commands in it */
    if (SC_OperData.AtsInfoTblAddr[NewAtsNum].NumberOfCommands > 0)
    {
        /*
         ** Stop the current ATS
         */
        SC_KillAts();
        
        /*
         ** Start up the other ATS
         */
        if (SC_BeginAts (NewAtsNum , 0))
        {
            SC_OperData.AtsCtrlBlckAddr -> AtpState = SC_STARTING;
            
            CFE_EVS_SendEvent(SC_ATS_INLINE_SWTCH_INF_EID,
                              CFE_EVS_INFORMATION ,
                              "ATS Switched from %c to %c",
                              (OldAtsNum?'B':'A'), (NewAtsNum?'B':'A'));
            
            /*
             **  Update the command counter and return code
             */
            SC_AppData.CmdCtr++;
            ReturnCode = TRUE;  
        }
        else
        { /* all of the commands in the new ats were skipped */
            
            /*
             ** update the command error counter
             */
            SC_AppData.CmdErrCtr++;
            ReturnCode = FALSE;
            
        }  /* end if */
    }
    else
    { /* the other ATS does not have any commands in it */
        CFE_EVS_SendEvent(SC_ATS_INLINE_SWTCH_NOT_LDED_ERR_EID,
                          CFE_EVS_ERROR  ,
                          "Switch ATS Failure: Destination ATS Not Loaded");
        /*
         ** update the ATS error counter
         */
        SC_AppData.CmdErrCtr++;
        ReturnCode = FALSE;
        
    }  /* end if */
    
    /* clear out the global ground-switch pend flag */
    SC_OperData.AtsCtrlBlckAddr -> SwitchPendFlag = FALSE;
    
    return (ReturnCode);
    
} /* end function */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Jump an ATS forward in time                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_JumpAtsCmd (CFE_SB_MsgPtr_t CmdPacket)
{
    SC_AtsEntryHeader_t   *Entry;         /* ATS table entry pointer */
    int32                  EntryIndex;    /* ATS entry location in table */
    SC_AbsTimeTag_t        JumpTime;      /* the time to jump to in the ATS */
    SC_AbsTimeTag_t        ListCmdTime;   /* list entry execution time */
    uint16                 AtsIndex;      /* index of the ATS that is running */
    int32                  TimeIndex;     /* the current time buffer index */
    int32                  CmdIndex;      /* ATS command index (cmd num - 1) */
    char                   TimeBuffer[CFE_TIME_PRINTED_STRING_SIZE];
    CFE_TIME_SysTime_t     NewTime;
    uint16                 NumSkipped;

    if (SC_VerifyCmdLength(CmdPacket, sizeof(SC_JumpAtsCmd_t)))
    { 
        if (SC_OperData.AtsCtrlBlckAddr -> AtpState == SC_EXECUTING)
        {        
            JumpTime = ((SC_JumpAtsCmd_t *)CmdPacket) -> NewTime ;
            AtsIndex = SC_OperData.AtsCtrlBlckAddr -> AtsNumber - 1;
            
            /*
             ** Loop through the commands until a time tag is found
             ** that has a time greater than or equal to the current time OR
             ** all of the commands have been skipped
             */
            TimeIndex = 0;
            NumSkipped = 0;
            
            while (TimeIndex < SC_OperData.AtsInfoTblAddr[AtsIndex].NumberOfCommands)
            {
                /* first get the cmd index at this list entry */
                CmdIndex = SC_AppData.AtsTimeIndexBuffer[AtsIndex][TimeIndex];
                /* then get the entry index from the cmd index table */
                EntryIndex = SC_AppData.AtsCmdIndexBuffer[AtsIndex][CmdIndex];
                /* then get a pointer to the ATS entry data */
                Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[AtsIndex][EntryIndex];
                /* then get cmd execution time from the ATS entry */
                ListCmdTime = SC_GetAtsEntryTime(Entry);

                /* compare ATS jump time to this list entry time */
                if (SC_CompareAbsTime(JumpTime, ListCmdTime))
                {
                    /* jump time is greater than this list entry time */

                    /*
                    ** If the ATS command is loaded and ready to run, then
                    **  mark the command as being skipped
                    **  if the command has any other status, SC_SKIPPED, SC_EXECUTED,
                    **   etc, then leave the status alone.
                    */
                    if (SC_OperData.AtsCmdStatusTblAddr[AtsIndex][CmdIndex] == SC_LOADED)
                    {
                        SC_OperData.AtsCmdStatusTblAddr[AtsIndex][CmdIndex] = SC_SKIPPED;
                        NumSkipped++;
                    }

                    TimeIndex++;
                }
                else
                {
                    /* jump time is less than or equal to this list entry */
                    break;
                }
            }
            
            /*
             ** Check to see if the whole ATS was skipped
             */
            if (TimeIndex == SC_OperData.AtsInfoTblAddr[AtsIndex].NumberOfCommands)
            {
                CFE_EVS_SendEvent(SC_JUMPATS_CMD_STOPPED_ERR_EID,
                                  CFE_EVS_ERROR,
                                  "Jump Cmd: All ATS commands were skipped, ATS stopped");
                
                SC_AppData.CmdErrCtr++;
                
                /* stop the ats */
                SC_KillAts();  
            }
            else
            {  /* there is at least one command to execute */
                
                /*
                 ** Update the ATP Control Block entries.
                 */
                SC_OperData.AtsCtrlBlckAddr -> CmdNumber = CmdIndex;
                SC_OperData.AtsCtrlBlckAddr -> TimeIndexPtr = TimeIndex;
                
                /*
                 ** Set the next command time for the ATP
                 */
                SC_AppData.NextCmdTime[SC_ATP] = ListCmdTime;
                
                SC_AppData.CmdCtr++;
                
                /* print out the date in a readable format */
                NewTime.Seconds = ListCmdTime;
                NewTime.Subseconds = 0;
                
                CFE_TIME_Print( (char *)&TimeBuffer, NewTime);
                
                CFE_EVS_SendEvent(SC_JUMP_ATS_INF_EID,
                                  CFE_EVS_INFORMATION,
                                  "Next ATS command time in the ATP was set to %s",
                                  TimeBuffer); 
                if (NumSkipped > 0)
                {
                    /* We skipped come commands, but not all of them */
                    CFE_EVS_SendEvent(SC_JUMP_ATS_SKIPPED_DBG_EID,
                                     CFE_EVS_DEBUG,
                                     "Jump Cmd: Skipped %d ATS commands",
                                     NumSkipped); 
                    
                }                  
                                  
            }  /* end if */
        }
        else
        { /*  There is not a running ATS */
            
            CFE_EVS_SendEvent(SC_JUMPATS_CMD_NOT_ACT_ERR_EID,
                              CFE_EVS_ERROR,
                              "ATS Jump Failed: No active ATS");
            SC_AppData.CmdErrCtr++;
            
        } /* end if */
    } 
} /* end SC_JumpAtsCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Continue ATS on Checksum Failure Cmd                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_ContinueAtsOnFailureCmd(CFE_SB_MsgPtr_t CmdPacket)
{
    uint16 State;
   
    
    if (SC_VerifyCmdLength(CmdPacket, sizeof(SC_SetContinueAtsOnFailureCmd_t)))
    {
        State = ((SC_SetContinueAtsOnFailureCmd_t *) CmdPacket) -> ContinueState;
        
        if (State != TRUE && State != FALSE)
        {
            SC_AppData.CmdErrCtr++;
                
            CFE_EVS_SendEvent(SC_CONT_CMD_ERR_EID,
                              CFE_EVS_ERROR,
                              "Continue ATS On Failure command  failed, invalid state: %d",
                              State);
        }
        else
        {
            SC_AppData.ContinueAtsOnFailureFlag = State;    
            
            SC_AppData.CmdCtr++;
                
            CFE_EVS_SendEvent(SC_CONT_CMD_DEB_EID,
                              CFE_EVS_DEBUG,
                              "Continue-ATS-On-Failure command, State: %d",
                              State);
        }
                
     }
}/* end SC_ContinueAtsOnFailureCmd */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Append to selected ATS                                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_AppendAtsCmd (CFE_SB_MsgPtr_t CmdPacket)
{
    SC_AppendAtsCmd_t *AppendCmd = (SC_AppendAtsCmd_t *) CmdPacket;
    uint16  AtsIndex;  /* index (not ID) of target ATS */

    if (SC_VerifyCmdLength(CmdPacket, sizeof(SC_AppendAtsCmd_t)))
    {
        /* create base zero array index from base one ID value */
        AtsIndex = AppendCmd->AtsId - 1;
        
        if (AtsIndex >= SC_NUMBER_OF_ATS)
        {
            /* invalid target ATS selection */
            SC_AppData.CmdErrCtr++;
            
            CFE_EVS_SendEvent(SC_APPEND_CMD_ARG_ERR_EID, CFE_EVS_ERROR,
                             "Append ATS error: invalid ATS ID = %d", AppendCmd->AtsId);
        }
        else if (SC_OperData.AtsInfoTblAddr[AtsIndex].NumberOfCommands == 0)
        {
            /* target ATS table is empty */
            SC_AppData.CmdErrCtr++;

            CFE_EVS_SendEvent(SC_APPEND_CMD_TGT_ERR_EID, CFE_EVS_ERROR,
                             "Append ATS %c error: ATS table is empty", 'A' + AtsIndex);
        }
        else if (SC_AppData.AppendEntryCount == 0)
        { 
            /* append table is empty */
            SC_AppData.CmdErrCtr++;
            
            CFE_EVS_SendEvent(SC_APPEND_CMD_SRC_ERR_EID, CFE_EVS_ERROR,
                             "Append ATS %c error: Append table is empty", 'A' + AtsIndex);
        }
        else if ((SC_OperData.AtsInfoTblAddr[AtsIndex].AtsSize + SC_AppData.AppendWordCount) > SC_ATS_BUFF_SIZE)
        { 
            /* not enough room in ATS buffer for Append table data */
            SC_AppData.CmdErrCtr++;
            
            CFE_EVS_SendEvent(SC_APPEND_CMD_FIT_ERR_EID, CFE_EVS_ERROR,
                             "Append ATS %c error: ATS size = %d, Append size = %d, ATS buffer = %d",
                             'A' + AtsIndex, SC_OperData.AtsInfoTblAddr[AtsIndex].AtsSize,
                              SC_AppData.AppendWordCount, SC_ATS_BUFF_SIZE);
        }
        else
        { 
            /* store ATS selection from most recent ATS Append command */
            SC_AppData.AppendCmdArg = AppendCmd->AtsId;

            /* copy append data and re-calc timing data */
            SC_ProcessAppend(AtsIndex);

            /* increment command success counter */
            SC_AppData.CmdCtr++;
                        
            CFE_EVS_SendEvent(SC_APPEND_CMD_INF_EID, CFE_EVS_INFORMATION,
                             "Append ATS %c command: %d ATS entries appended",
                             'A' + AtsIndex, SC_AppData.AppendEntryCount);
        }
    }
} /* end SC_AppendAtsCmd */

/************************/
/*  End of File Comment */
/************************/
