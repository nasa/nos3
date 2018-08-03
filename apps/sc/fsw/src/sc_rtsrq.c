 /*************************************************************************
 ** File:
 **   $Id: sc_rtsrq.c 1.11 2015/03/02 12:58:58EST sstrege Exp  $
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
 **     This file contains functions to handle all of the RTS
 **     executive requests and internal reuqests to control
 **     the RTP and RTSs.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sc_rtsrq.c  $
 **   Revision 1.11 2015/03/02 12:58:58EST sstrege 
 **   Added copyright information
 **   Revision 1.10 2014/06/06 11:37:58EDT sjudy 
 **   Changed event msgs to have 'RTS' or 'ATS' instead of "Real Time Sequence", etc.
 **   Revision 1.9 2011/09/23 10:27:05GMT-08:00 lwalling 
 **   Made group commands conditional on configuration definition
 **   Revision 1.8 2011/09/07 11:15:11EDT lwalling 
 **   Fix group cmd event text for invalid RTS ID
 **   Revision 1.7 2011/03/14 10:53:15EDT lwalling 
 **   Add new command handlers -- SC_StartRtsGrpCmd(), SC_StopRtsGrpCmd(), SC_DisableGrpCmd(), SC_EnableGrpCmd().
 **   Revision 1.6 2010/09/28 10:33:09EDT lwalling 
 **   Update list of included header files
 **   Revision 1.5 2010/05/18 15:30:23EDT lwalling 
 **   Change AtsId/RtsId to AtsIndex/RtsIndex or AtsNumber/RtsNumber
 **   Revision 1.4 2010/03/26 18:03:01EDT lwalling 
 **   Remove pad from ATS and RTS structures, change 32 bit ATS time to two 16 bit values
 **   Revision 1.3 2009/01/26 14:47:15EST nyanchik 
 **   Check in of Unit test
 **   Revision 1.2 2009/01/05 08:26:56EST nyanchik 
 **   Check in after code review changes
 *************************************************************************/
 
/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/

#include "cfe.h"
#include "sc_app.h"
#include "sc_rtsrq.h"
#include "sc_utils.h"
#include "sc_events.h"
#include "sc_msgids.h"

/**************************************************************************
 **
 ** Functions
 **
 **************************************************************************/


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Starts and RTS                                                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_StartRtsCmd (CFE_SB_MsgPtr_t CmdPacket)
{
    
    uint16                         RtsIndex;    /* rts array index */
    CFE_SB_MsgPtr_t                RtsEntryCmd; /* pointer to an rts command */
    SC_RtsEntryHeader_t           *RtsEntryPtr;
    uint16                         CmdLength;   /* the length of the 1st cmd */

    
    /*
     ** Verify command packet length...
     */
    if (SC_VerifyCmdLength(CmdPacket, sizeof(SC_RtsCmd_t)))
    {
        /* convert RTS number to RTS array index */
        RtsIndex = ((SC_RtsCmd_t *)CmdPacket) -> RtsId - 1;
        /*
         ** Check start RTS parameters
         */
        if (RtsIndex < SC_NUMBER_OF_RTS)
        {
            /* make sure that RTS is not disabled */
            if (SC_OperData.RtsInfoTblAddr[RtsIndex].DisabledFlag == FALSE)
            {
                /* the requested RTS is not being used and is not empty */
                if (SC_OperData.RtsInfoTblAddr[RtsIndex].RtsStatus == SC_LOADED)
                {               
                    /*
                     ** Check the command length
                     */   
                    RtsEntryPtr = (SC_RtsEntryHeader_t *) SC_OperData.RtsTblAddr[RtsIndex];
                    RtsEntryCmd = (CFE_SB_MsgPtr_t) RtsEntryPtr->CmdHeader;
                    
                    CmdLength = CFE_SB_GetTotalMsgLength(RtsEntryCmd); 
                     /* Make sure the command is big enough, but not too big  */
                    if (CmdLength >= SC_PACKET_MIN_SIZE  && CmdLength <= SC_PACKET_MAX_SIZE)
                    {                        
                        /*
                         **  Initialize the RTS info table entry
                         */
                        SC_OperData.RtsInfoTblAddr[RtsIndex].RtsStatus = SC_EXECUTING;
                        SC_OperData.RtsInfoTblAddr[RtsIndex].CmdCtr = 0;
                        SC_OperData.RtsInfoTblAddr[RtsIndex].CmdErrCtr = 0;
                        SC_OperData.RtsInfoTblAddr[RtsIndex].NextCommandPtr = 0;
                        SC_OperData.RtsInfoTblAddr[RtsIndex].UseCtr ++;
                        
                        /*
                         ** Get the absolute time for the RTSs next_cmd_time
                         ** using the current time and the relative time tag.
                         */
                        SC_OperData.RtsInfoTblAddr[RtsIndex].NextCommandTime  = 
                            SC_ComputeAbsTime(RtsEntryPtr->TimeTag);

                        
                        /*
                         ** Last, Increment some global counters associated with the
                         ** starting of the RTS
                         */
                        SC_OperData.RtsCtrlBlckAddr -> NumRtsActive++;
                        SC_AppData.RtsActiveCtr++;
                        SC_AppData.CmdCtr++;
                        
                        if (((SC_RtsCmd_t *)CmdPacket) -> RtsId <= SC_LAST_RTS_WITH_EVENTS)
                        {
                            CFE_EVS_SendEvent (SC_RTS_START_INF_EID,
                                               CFE_EVS_INFORMATION,
                                               "RTS Number %03d Started",
                                               ((SC_RtsCmd_t *)CmdPacket) -> RtsId);
                        }
                        else
                        {
                            CFE_EVS_SendEvent(SC_STARTRTS_CMD_DBG_EID,
                                              CFE_EVS_DEBUG,
                                              "Start RTS #%d command",
                                              ((SC_RtsCmd_t *)CmdPacket) -> RtsId);
                        }
                    }
                    else
                    { /* the length field of the 1st cmd was bad */
                        CFE_EVS_SendEvent (SC_STARTRTS_CMD_INVLD_LEN_ERR_EID,
                                           CFE_EVS_ERROR,
                                           "Start RTS %03d Rejected: Invld Len Field for 1st Cmd in Sequence. Invld Cmd Length = %d",
                                           ((SC_RtsCmd_t *)CmdPacket) -> RtsId,
                                           CmdLength);
                        
                        SC_AppData.CmdErrCtr++;
                        SC_AppData.RtsActiveErrCtr++;
                        
                    } /* end if - check command number */
                }
                else
                {  /* Cannot use the RTS now */
                    
                    CFE_EVS_SendEvent (SC_STARTRTS_CMD_NOT_LDED_ERR_EID,
                                       CFE_EVS_ERROR,
                                       "Start RTS %03d Rejected: RTS Not Loaded or In Use, Status: %d",
                                       ((SC_RtsCmd_t *)CmdPacket) -> RtsId,
                                       SC_OperData.RtsInfoTblAddr[RtsIndex].RtsStatus);
                    
                    SC_AppData.CmdErrCtr++;
                    SC_AppData.RtsActiveErrCtr++;
                    
                    
                } /* end if */
            }
            else
            {  /* the RTS is disabled */
                CFE_EVS_SendEvent (SC_STARTRTS_CMD_DISABLED_ERR_EID,
                                   CFE_EVS_ERROR,
                                   "Start RTS %03d Rejected: RTS Disabled",
                                   ((SC_RtsCmd_t *)CmdPacket) -> RtsId);
                
                SC_AppData.CmdErrCtr++;
                SC_AppData.RtsActiveErrCtr++;
                
            } /* end if */
        }
        else
        {     /* the rts id is invalid */
            CFE_EVS_SendEvent (SC_STARTRTS_CMD_INVALID_ERR_EID,
                               CFE_EVS_ERROR,
                               "Start RTS %03d Rejected: Invalid RTS ID",
                               ((SC_RtsCmd_t *)CmdPacket) -> RtsId);
            
            SC_AppData.CmdErrCtr++;
            SC_AppData.RtsActiveErrCtr++;
            
        }
    }
    else
    {     /* the command length is invalid */
        SC_AppData.RtsActiveErrCtr++;
    }
    
} /* end SC_StartRts */


#if (SC_ENABLE_GROUP_COMMANDS == TRUE)
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Start a group of RTS                                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_StartRtsGrpCmd (CFE_SB_MsgPtr_t CmdPacket)
{
    uint16 FirstIndex;   /* RTS array index */
    uint16 LastIndex;
    uint16 RtsIndex;
    int32  StartCount = 0;

    if (SC_VerifyCmdLength(CmdPacket, sizeof(SC_RtsGrpCmd_t)))
    {
        /* convert RTS number to RTS array index */
        FirstIndex = ((SC_RtsGrpCmd_t *)CmdPacket)->FirstRtsId - 1;
        LastIndex  = ((SC_RtsGrpCmd_t *)CmdPacket)->LastRtsId  - 1;

        /* make sure the specified group is valid */
        if ((FirstIndex < SC_NUMBER_OF_RTS) &&
            (LastIndex  < SC_NUMBER_OF_RTS) &&
            (FirstIndex <= LastIndex))
        {
            for (RtsIndex = FirstIndex; RtsIndex <= LastIndex; RtsIndex++)
            {
                /* make sure that RTS is not disabled, empty or executing */
                if ((SC_OperData.RtsInfoTblAddr[RtsIndex].DisabledFlag == FALSE) &&
                    (SC_OperData.RtsInfoTblAddr[RtsIndex].RtsStatus == SC_LOADED))
                {               
                    /* initialize the RTS info table entry */
                    SC_OperData.RtsInfoTblAddr[RtsIndex].RtsStatus = SC_EXECUTING;
                    SC_OperData.RtsInfoTblAddr[RtsIndex].CmdCtr = 0;
                    SC_OperData.RtsInfoTblAddr[RtsIndex].CmdErrCtr = 0;
                    SC_OperData.RtsInfoTblAddr[RtsIndex].NextCommandPtr = 0;
                    SC_OperData.RtsInfoTblAddr[RtsIndex].UseCtr ++;
                        
                    /* get absolute time for 1st cmd in the RTS */
                    SC_OperData.RtsInfoTblAddr[RtsIndex].NextCommandTime  = 
                       SC_ComputeAbsTime(((SC_RtsEntryHeader_t *) SC_OperData.RtsTblAddr[RtsIndex])->TimeTag);

                    /* maintain counters associated with starting RTS */
                    SC_OperData.RtsCtrlBlckAddr->NumRtsActive++;
                    SC_AppData.RtsActiveCtr++;
                    SC_AppData.CmdCtr++;
                        
                    /* count the RTS that were actually started */
                    StartCount++;
                }
            }            

            /* success */
            CFE_EVS_SendEvent (SC_STARTRTSGRP_CMD_INF_EID, CFE_EVS_INFORMATION,
                               "Start RTS group: FirstID=%d, LastID=%d, Modified=%d",
                              ((SC_RtsGrpCmd_t *)CmdPacket)->FirstRtsId,
                              ((SC_RtsGrpCmd_t *)CmdPacket)->LastRtsId, StartCount);
            SC_AppData.CmdCtr++;
        }
        else
        {   /* error */
            CFE_EVS_SendEvent (SC_STARTRTSGRP_CMD_ERR_EID, CFE_EVS_ERROR,
                               "Start RTS group error: FirstID=%d, LastID=%d",
                              ((SC_RtsGrpCmd_t *)CmdPacket)->FirstRtsId,
                              ((SC_RtsGrpCmd_t *)CmdPacket)->LastRtsId);
            SC_AppData.CmdErrCtr++;
        }
    }

    return;

} /* end SC_StartRtsGrpCmd */
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Stop an RTS                                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_StopRtsCmd (CFE_SB_MsgPtr_t CmdPacket)
{
    uint16      RtsIndex;   /* RTS array index */

    if (SC_VerifyCmdLength(CmdPacket, sizeof(SC_RtsCmd_t)))
    {
        /* convert RTS number to RTS array index */
        RtsIndex = ((SC_RtsCmd_t *)CmdPacket) -> RtsId - 1;
        
        /* check the command parameter */
        if (RtsIndex < SC_NUMBER_OF_RTS)
        {
            /* stop the rts by calling a generic routine */
            SC_KillRts (RtsIndex);
            
            SC_AppData.CmdCtr++;
            
            CFE_EVS_SendEvent (SC_STOPRTS_CMD_INF_EID,
                               CFE_EVS_INFORMATION,
                               "RTS %03d Aborted",
                               ((SC_RtsCmd_t *)CmdPacket) -> RtsId);
        }
        else
        {/* the specified RTS is invalid */
            
            /* the rts id is invalid */
            CFE_EVS_SendEvent (SC_STOPRTS_CMD_ERR_EID,
                               CFE_EVS_ERROR,
                               "Stop RTS %03d rejected: Invalid RTS ID",
                               ((SC_RtsCmd_t *)CmdPacket) -> RtsId);
            
            SC_AppData.CmdErrCtr++;
            
        } /* end if */
    }
} /* end SC_StopRtsCmd */


#if (SC_ENABLE_GROUP_COMMANDS == TRUE)
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Stop a group of RTS                                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_StopRtsGrpCmd (CFE_SB_MsgPtr_t CmdPacket)
{
    uint16 FirstIndex;   /* RTS array index */
    uint16 LastIndex;
    uint16 RtsIndex;
    int32  StopCount = 0;

    if (SC_VerifyCmdLength(CmdPacket, sizeof(SC_RtsGrpCmd_t)))
    {
        /* convert RTS number to RTS array index */
        FirstIndex = ((SC_RtsGrpCmd_t *)CmdPacket)->FirstRtsId - 1;
        LastIndex  = ((SC_RtsGrpCmd_t *)CmdPacket)->LastRtsId  - 1;

        /* make sure the specified group is valid */
        if ((FirstIndex < SC_NUMBER_OF_RTS) &&
            (LastIndex  < SC_NUMBER_OF_RTS) &&
            (FirstIndex <= LastIndex))
        {
            for (RtsIndex = FirstIndex; RtsIndex <= LastIndex; RtsIndex++)
            {
                /* count the entries that were actually stopped */
                if (SC_OperData.RtsInfoTblAddr[RtsIndex].RtsStatus == SC_EXECUTING)
                {
                    SC_KillRts(RtsIndex);
                    StopCount++;
                }
            }            

            /* success */
            CFE_EVS_SendEvent (SC_STOPRTSGRP_CMD_INF_EID, CFE_EVS_INFORMATION,
                               "Stop RTS group: FirstID=%d, LastID=%d, Modified=%d",
                              ((SC_RtsGrpCmd_t *)CmdPacket)->FirstRtsId,
                              ((SC_RtsGrpCmd_t *)CmdPacket)->LastRtsId, StopCount);
            SC_AppData.CmdCtr++;
        }
        else
        {   /* error */
            CFE_EVS_SendEvent (SC_STOPRTSGRP_CMD_ERR_EID, CFE_EVS_ERROR,
                               "Stop RTS group error: FirstID=%d, LastID=%d",
                              ((SC_RtsGrpCmd_t *)CmdPacket)->FirstRtsId,
                              ((SC_RtsGrpCmd_t *)CmdPacket)->LastRtsId);
            SC_AppData.CmdErrCtr++;
        }
    }

    return;

} /* end SC_StopRtsGrpCmd */
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Disables an RTS                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_DisableRtsCmd (CFE_SB_MsgPtr_t CmdPacket)
{
    uint16      RtsIndex;   /* RTS array index */

    if (SC_VerifyCmdLength(CmdPacket, sizeof(SC_RtsCmd_t)))
    {
        /* convert RTS number to RTS array index */
        RtsIndex = ((SC_RtsCmd_t *)CmdPacket) -> RtsId - 1;
        
        /* make sure tha specified rts is valid */
        if (RtsIndex < SC_NUMBER_OF_RTS)
        {
            
            /* disable the RTS */
            SC_OperData.RtsInfoTblAddr[RtsIndex].DisabledFlag = TRUE;
            
            /* update the command status */
            SC_AppData.CmdCtr++;
            
            CFE_EVS_SendEvent (SC_DISABLE_RTS_DEB_EID,
                               CFE_EVS_DEBUG,
                               "Disabled RTS %03d",
                               ((SC_RtsCmd_t *)CmdPacket) -> RtsId);   
        }
        else
        {   /* it is not a valid RTS id */
            CFE_EVS_SendEvent (SC_DISRTS_CMD_ERR_EID,
                               CFE_EVS_ERROR,
                               "Disable RTS %03d Rejected: Invalid RTS ID",
                               ((SC_RtsCmd_t *)CmdPacket) -> RtsId);
            
            /* update the command error status */
            SC_AppData.CmdErrCtr++;     
        } /* end if */
    } 
} /* end SC_DisableRTS */


#if (SC_ENABLE_GROUP_COMMANDS == TRUE)
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Disable a group of RTS                                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_DisableRtsGrpCmd (CFE_SB_MsgPtr_t CmdPacket)
{
    uint16 FirstIndex;   /* RTS array index */
    uint16 LastIndex;
    uint16 RtsIndex;
    int32  DisableCount = 0;

    if (SC_VerifyCmdLength(CmdPacket, sizeof(SC_RtsGrpCmd_t)))
    {
        /* convert RTS number to RTS array index */
        FirstIndex = ((SC_RtsGrpCmd_t *)CmdPacket)->FirstRtsId - 1;
        LastIndex  = ((SC_RtsGrpCmd_t *)CmdPacket)->LastRtsId  - 1;

        /* make sure the specified group is valid */
        if ((FirstIndex < SC_NUMBER_OF_RTS) &&
            (LastIndex  < SC_NUMBER_OF_RTS) &&
            (FirstIndex <= LastIndex))
        {
            for (RtsIndex = FirstIndex; RtsIndex <= LastIndex; RtsIndex++)
            {
                /* count the entries that were actually disabled */
                if (SC_OperData.RtsInfoTblAddr[RtsIndex].DisabledFlag == FALSE)
                {
                    DisableCount++;
                    SC_OperData.RtsInfoTblAddr[RtsIndex].DisabledFlag = TRUE;
                }
            }            

            /* success */
            CFE_EVS_SendEvent (SC_DISRTSGRP_CMD_INF_EID, CFE_EVS_INFORMATION,
                               "Disable RTS group: FirstID=%d, LastID=%d, Modified=%d",
                              ((SC_RtsGrpCmd_t *)CmdPacket)->FirstRtsId,
                              ((SC_RtsGrpCmd_t *)CmdPacket)->LastRtsId, DisableCount);
            SC_AppData.CmdCtr++;
        }
        else
        {   /* error */
            CFE_EVS_SendEvent (SC_DISRTSGRP_CMD_ERR_EID, CFE_EVS_ERROR,
                               "Disable RTS group error: FirstID=%d, LastID=%d",
                              ((SC_RtsGrpCmd_t *)CmdPacket)->FirstRtsId,
                              ((SC_RtsGrpCmd_t *)CmdPacket)->LastRtsId);
            SC_AppData.CmdErrCtr++;
        }
    }

    return;

} /* end SC_DisableRtsGrpCmd */
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Enables an RTS                                                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_EnableRtsCmd (CFE_SB_MsgPtr_t CmdPacket)
{
    uint16      RtsIndex;   /* RTS array index */

    if (SC_VerifyCmdLength(CmdPacket, sizeof(SC_RtsCmd_t)))
    {
        /* convert RTS number to RTS array index */
        RtsIndex = ((SC_RtsCmd_t *)CmdPacket) -> RtsId - 1;

        /* make sure the specified rts is valid */
        if (RtsIndex < SC_NUMBER_OF_RTS)
        {
            
            /* re-enable the RTS */
            SC_OperData.RtsInfoTblAddr[RtsIndex].DisabledFlag = FALSE;
            
            /* update the command status */
            SC_AppData.CmdCtr++;
            
            CFE_EVS_SendEvent (SC_ENABLE_RTS_DEB_EID,
                               CFE_EVS_DEBUG ,
                               "Enabled RTS %03d",
                               ((SC_RtsCmd_t *)CmdPacket) -> RtsId);
            
            
        }
        else
        {   /* it is not a valid RTS id */
            CFE_EVS_SendEvent (SC_ENARTS_CMD_ERR_EID,
                               CFE_EVS_ERROR,
                               "Enable RTS %03d Rejected: Invalid RTS ID",
                               ((SC_RtsCmd_t *)CmdPacket) -> RtsId);
            
            /* update the command error status */
            SC_AppData.CmdErrCtr++;
            
            
        } /* end if */
    }
} /* end SC_EnableRTS */


#if (SC_ENABLE_GROUP_COMMANDS == TRUE)
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Enable a group of RTS                                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_EnableRtsGrpCmd (CFE_SB_MsgPtr_t CmdPacket)
{
    uint16 FirstIndex;   /* RTS array index */
    uint16 LastIndex;
    uint16 RtsIndex;
    int32  EnableCount = 0;

    if (SC_VerifyCmdLength(CmdPacket, sizeof(SC_RtsGrpCmd_t)))
    {
        /* convert RTS number to RTS array index */
        FirstIndex = ((SC_RtsGrpCmd_t *)CmdPacket)->FirstRtsId - 1;
        LastIndex  = ((SC_RtsGrpCmd_t *)CmdPacket)->LastRtsId  - 1;

        /* make sure the specified group is valid */
        if ((FirstIndex < SC_NUMBER_OF_RTS) &&
            (LastIndex  < SC_NUMBER_OF_RTS) &&
            (FirstIndex <= LastIndex))
        {
            for (RtsIndex = FirstIndex; RtsIndex <= LastIndex; RtsIndex++)
            {
                /* count the entries that were actually enabled */
                if (SC_OperData.RtsInfoTblAddr[RtsIndex].DisabledFlag == TRUE)
                {
                    EnableCount++;
                    SC_OperData.RtsInfoTblAddr[RtsIndex].DisabledFlag = FALSE;
                }
            }            

            /* success */
            CFE_EVS_SendEvent (SC_ENARTSGRP_CMD_INF_EID, CFE_EVS_INFORMATION,
                               "Enable RTS group: FirstID=%d, LastID=%d, Modified=%d",
                              ((SC_RtsGrpCmd_t *)CmdPacket)->FirstRtsId,
                              ((SC_RtsGrpCmd_t *)CmdPacket)->LastRtsId, EnableCount);
            SC_AppData.CmdCtr++;
        }
        else
        {   /* error */
            CFE_EVS_SendEvent (SC_ENARTSGRP_CMD_ERR_EID, CFE_EVS_ERROR,
                               "Enable RTS group error: FirstID=%d, LastID=%d",
                              ((SC_RtsGrpCmd_t *)CmdPacket)->FirstRtsId,
                              ((SC_RtsGrpCmd_t *)CmdPacket)->LastRtsId);
            SC_AppData.CmdErrCtr++;
        }
    }

    return;

} /* end SC_EnableRtsGrpCmd */
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  Kill an RTS and clear out its data                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_KillRts (uint16 RtsIndex)
{    
    if (SC_OperData.RtsInfoTblAddr[RtsIndex].RtsStatus == SC_EXECUTING)
    {
        /*
         ** Stop the RTS from executing
         */
        SC_OperData.RtsInfoTblAddr[RtsIndex].RtsStatus = SC_LOADED;
        SC_OperData.RtsInfoTblAddr[RtsIndex].NextCommandTime = SC_MAX_TIME;
        
        /*
         ** Note: the rest of the fields are left alone
         ** to provide information on where the
         ** rts stopped. They are cleared out when it is restarted.
         */
        
        if (SC_OperData.RtsCtrlBlckAddr -> NumRtsActive > 0)
        {
            SC_OperData.RtsCtrlBlckAddr -> NumRtsActive--;
        }
    }

} /* end SC_KillRts */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Start an RTS on initilization                                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_AutoStartRts (uint16 RtsNumber)
{
    SC_RtsCmd_t   CmdPkt;    /* the command packet to start an RTS */

    /*
     ** Format the command packet to start the first RTS
     */
    
    CFE_SB_InitMsg(&CmdPkt, SC_CMD_MID, sizeof(SC_RtsCmd_t), TRUE);
    
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)  &CmdPkt, SC_START_RTS_CC);
    
    /*
     ** Get the RTS ID to start.
     */
    CmdPkt.RtsId = RtsNumber;
    
    /*
     ** Now send the command back to SC
     */
    CFE_SB_SendMsg((CFE_SB_MsgPtr_t)((int)&CmdPkt));
       
} /* end SC_AutoStartRts */

/************************/
/*  End of File Comment */
/************************/
