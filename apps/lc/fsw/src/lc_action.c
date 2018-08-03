/*************************************************************************
** File:
**   $Id: lc_action.c 1.3.1.2 2015/03/04 16:15:46EST sstrege Exp  $
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
**   Functions used for CFS Limit Checker actionpoint processing
**
**   $Log: lc_action.c  $
**   Revision 1.3.1.2 2015/03/04 16:15:46EST sstrege 
**   Added copyright information
**   Revision 1.3.1.1 2012/10/01 18:40:07EDT lwalling 
**   Apply 1.4 changes to branch
**   Revision 1.4 2012/10/01 13:20:48PDT lwalling 
**   Removed unused variable Operand from function LC_EvaluateRPN()
**   Revision 1.3 2012/08/01 11:41:07PDT lwalling 
**   Cleanup actionpoint operators use of STALE
**   Revision 1.2 2012/08/01 11:20:22PDT lwalling 
**   Change NOT_MEASURED to STALE
**   Revision 1.1 2012/07/31 13:53:36PDT nschweis 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lcx/fsw/src/project.pj
**   Revision 1.10 2012/06/26 17:59:09EDT lwalling 
**   Fix ap logic for OR and AND operators when wp result = LC_WATCH_NOT_MEASURED
**   Revision 1.9 2011/02/07 14:58:33PST lwalling 
**   Modify sample AP commands to target groups of AP's
**   Revision 1.8 2011/01/19 12:45:39EST jmdagost 
**   Moved two message parameters to the message IDs file for scheduler table access.
**   Revision 1.7 2011/01/19 11:35:21EST jmdagost 
**   Initialize local variables RPNIndex and RPNStackDepth.
**   Revision 1.6 2010/04/12 14:29:31EDT lwalling 
**   Changed bitwise RPN comparisons to logical comparisons
**   Revision 1.5 2010/02/19 17:43:54EST lwalling 
**   Change state events limits to individual limits for Passive AP, FailToPass and PassToFail
**   Revision 1.4 2009/12/28 14:47:21EST lwalling 
**   Add event limits, change limited events from debug to info
**   Revision 1.3 2009/01/29 15:39:03EST dahardis 
**   Changed an event message from INFO to DEBUG as documented
**   in DCR #6811
**   Revision 1.2 2008/12/03 13:59:46EST dahardis 
**   Corrections from peer code review
**   Revision 1.1 2008/10/29 14:18:36EDT dahardison 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lc/fsw/src/project.pj
** 
*************************************************************************/

/*************************************************************************
** Includes
*************************************************************************/
#include "lc_app.h"
#include "lc_action.h"
#include "lc_msgids.h"
#include "lc_events.h"
#include "lc_custom.h"

#include <string.h>

/*************************************************************************
** Local Function Prototypes
*************************************************************************/
/************************************************************************/
/** \brief Sample single actionpoint
**  
**  \par Description
**       Support function for actionpoint processing that will sample
**       a single actionpoint and handle the result as needed
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   APNumber     The actionpoint number to sample (zero
**                             based actionpoint definition table index)
**
*************************************************************************/
void LC_SampleSingleAP(uint16 APNumber);

/************************************************************************/
/** \brief Evaluate RPN
**  
**  \par Description
**       Support function for actionpoint processing that evaluates
**       the reverse polish notation (RPN) equation for the specified
**       actionpoint and returns the result
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   APNumber     The actionpoint number to evaluate (zero
**                             based actionpoint definition table index)
**
**  \returns
**  \retcode #LC_ACTION_PASS         \retdesc \copydoc LC_ACTION_PASS   \endcode
**  \retcode #LC_ACTION_FAIL         \retdesc \copydoc LC_ACTION_FAIL   \endcode
**  \retcode #LC_ACTION_STALE        \retdesc \copydoc LC_ACTION_STALE  \endcode
**  \retcode #LC_ACTION_ERROR        \retdesc \copydoc LC_ACTION_ERROR  \endcode
**  \endreturns
**
*************************************************************************/
uint8 LC_EvaluateRPN(uint16 APNumber);
 
/************************************************************************/
/** \brief Validate RPN expression
**  
**  \par Description
**       Support function for actionpoint definition table validation
**       that checks a reverse polish notation (RPN) equation for
**       possible errors.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   RPNPtr            Pointer to the RPN equation
**
**  \param [in]   IndexValue        A pointer where to store the equation
**                                  index value if an error is detected
**
**  \param [in]   StackDepthValue   A pointer where to store the equation
**                                  stack depth value if an error is detected
**
**  \param [out]  *IndexValue       Equation index value where error was
**                                  found. Not modified if return code is
**                                  #LC_ADTVAL_NO_ERR
**
**  \param [out]  *StackDepthValue  Equation stack depth value where error
**                                  found. Not modified if return code is
**                                  #LC_ADTVAL_NO_ERR
**
**  \returns
**  \retcode #LC_ADTVAL_NO_ERR   \retdesc \copydoc LC_ADTVAL_NO_ERR  \endcode
**  \retcode #LC_ADTVAL_ERR_RPN  \retdesc \copydoc LC_ADTVAL_ERR_RPN \endcode
**  \endreturns
**
**  \sa #LC_ValidateADT
**
*************************************************************************/
int32 LC_ValidateRPN(uint16 *RPNPtr, 
                      int32  *IndexValue, 
                      int32  *StackDepthValue);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Sample one or all actionpoints                                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */   
void LC_SampleAPs(uint16 StartIndex, uint16 EndIndex)
{
    uint16           TableIndex;
    uint8            CurrentAPState;

    /*
    ** If we're specifying a single actionpoint, make sure it's
    ** current state is valid for a sample request
    */
    if (StartIndex == EndIndex)
    {
        CurrentAPState = LC_OperData.ARTPtr[StartIndex].CurrentState;
         
        if ((CurrentAPState != LC_ACTION_NOT_USED) &&
            (CurrentAPState != LC_APSTATE_PERMOFF))
        {
            /*
            ** Sample the specified actionpoint
            */
            LC_SampleSingleAP(StartIndex);
        }
        else
        {
            /*
            **  Actionpoint isn't currently operational 
            */
            CFE_EVS_SendEvent(LC_APSAMPLE_CURR_ERR_EID, CFE_EVS_ERROR,
                              "Sample AP error, invalid current AP state: AP = %d, State = %d", 
                              StartIndex, CurrentAPState);
        }
    }
    else
    {
        /*
        ** Sample selected actionpoints
        */
        for (TableIndex = StartIndex; TableIndex <= EndIndex; TableIndex++)
        {
            LC_SampleSingleAP(TableIndex);
        }
        
    }

    return;
    
} /* end LC_SampleAP */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Sample a single actionpoint                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */   
void LC_SampleSingleAP(uint16 APNumber)
{
    uint8  CurrentAPState;
    uint8  PreviousResult;
    uint8  CurrentResult;
    char   EventText[CFE_EVS_MAX_MESSAGE_LENGTH];
     
    /*
    ** We only do the sample if the actionpoint is active
    ** or passive. Other states are ignored since this
    ** routine is called in a loop to process ALL actionpoints.
    */
    CurrentAPState = LC_OperData.ARTPtr[APNumber].CurrentState;
    
    if ((CurrentAPState == LC_APSTATE_ACTIVE) ||
        (CurrentAPState == LC_APSTATE_PASSIVE))
    {
        /*
        ** Evaluate the actionpoint and update the results
        ** as needed
        */
        LC_AppData.APSampleCount++;
        
        PreviousResult = LC_OperData.ARTPtr[APNumber].ActionResult;
        
        CurrentResult = LC_EvaluateRPN(APNumber);

        LC_OperData.ARTPtr[APNumber].ActionResult = CurrentResult;
        
        /*****************************************
        ** If actionpoint fails current evaluation
        ******************************************/
        if (CurrentResult == LC_ACTION_FAIL)
            {
            LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount++;
            LC_OperData.ARTPtr[APNumber].CumulativeFailCount++;
            
            if (PreviousResult == LC_ACTION_PASS)
                {
                /*
                **  We failed this time, but we passed last time
                */
                LC_OperData.ARTPtr[APNumber].PassToFailCount++;

                /*
                **  Send only a limited number of Pass to Fail events
                */
                if (LC_OperData.ARTPtr[APNumber].PassToFailCount <=
                    LC_OperData.ADTPtr[APNumber].MaxPassFailEvents)
                    {
                    CFE_EVS_SendEvent(LC_AP_PASSTOFAIL_INF_EID, CFE_EVS_INFORMATION,
                                     "AP state change from PASS to FAIL: AP = %d", 
                                      APNumber);
                    }
                }

            if (LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount >=
                LC_OperData.ADTPtr[APNumber].MaxFailsBeforeRTS)
                {
                /* 
                ** We have failed enough times to request the RTS 
                */
                if (CurrentAPState == LC_APSTATE_ACTIVE)
                    {
                    /* 
                    ** Actions go to passive after they've failed 
                    */
                    LC_OperData.ARTPtr[APNumber].CurrentState = LC_APSTATE_PASSIVE;

                    if (LC_AppData.CurrentLCState == LC_STATE_ACTIVE)
                        {
                        /*
                        ** If the LC application state is active, request the 
                        ** specified RTS be executed
                        */
                        LC_ExecuteRTS(LC_OperData.ADTPtr[APNumber].RTSId);

                        LC_OperData.ARTPtr[APNumber].CumulativeRTSExecCount++;

                        LC_AppData.RTSExecCount++;

                        /* 
                        ** Copy event text specific to this action
                        ** making sure to NUL terminate in case the event
                        ** text is too long since strncpy won't
                        */
                        strncpy(EventText,
                                LC_OperData.ADTPtr[APNumber].EventText,
                                LC_MAX_ACTION_TEXT);
                        EventText[LC_MAX_ACTION_TEXT - 1] = '\0';

                        /*
                        ** Add our trailer with AP specific info making
                        ** sure we won't exceed our character buffer.
                        ** strncat will always NUL terminate so we have
                        ** to subtract 1 from the max to leave room for it.
                        */
                        strncat(EventText, 
                                LC_AP_EVENT_TAIL_STR,
                                (CFE_EVS_MAX_MESSAGE_LENGTH - LC_MAX_ACTION_TEXT) - 1);

                        CFE_EVS_SendEvent(
                            LC_OperData.ADTPtr[APNumber].EventID,
                            LC_OperData.ADTPtr[APNumber].EventType,
                            EventText, APNumber,
                            LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount,
                            LC_OperData.ADTPtr[APNumber].RTSId);
                        }
                    else
                        {
                        /*
                        ** The LC application state is passive so we don't
                        ** do the RTS request. We bump the passive RTS execution 
                        ** counter and send out a generic event message
                        */
                        LC_AppData.PassiveRTSExecCount++;

                        CFE_EVS_SendEvent(LC_PASSIVE_FAIL_DBG_EID, CFE_EVS_DEBUG,
                            "AP failed while LC App passive: AP = %d, FailCount = %d, RTS = %d",
                            APNumber,
                            LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount,
                            LC_OperData.ADTPtr[APNumber].RTSId);
                        }
                    }
                else
                    {
                    /*
                    ** The actionpoint failed while the actionpoint state is passive
                    */
                    LC_OperData.ARTPtr[APNumber].PassiveAPCount++;

                    /*
                    **  Send only a limited number of AP is Passive events
                    */
                if (LC_OperData.ARTPtr[APNumber].PassiveAPCount <=
                    LC_OperData.ADTPtr[APNumber].MaxPassiveEvents)
                    {
                        CFE_EVS_SendEvent(LC_AP_PASSIVE_FAIL_INF_EID, CFE_EVS_INFORMATION,
                            "AP failed while passive: AP = %d, FailCount = %d, RTS = %d",
                            APNumber,
                            LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount,
                            LC_OperData.ADTPtr[APNumber].RTSId);
                        }
                    }

                } /* end (ConsecutiveFailCount >= MaxFailsBeforeRTS) if */
            
            } /* end (CurrentResult == LC_ACTION_FAIL) if */
        
        /******************************************
        ** If actionpoint passes current evaluation
        *******************************************/
        else if (CurrentResult == LC_ACTION_PASS)
        {
            if (PreviousResult == LC_ACTION_FAIL)
            {
                /*
                **  We passed this time, but we failed last time
                */
                LC_OperData.ARTPtr[APNumber].FailToPassCount++;

                /*
                **  Send only a limited number of Fail to Pass events
                */
                if (LC_OperData.ARTPtr[APNumber].FailToPassCount <=
                    LC_OperData.ADTPtr[APNumber].MaxFailPassEvents)
                    {
                    CFE_EVS_SendEvent(LC_AP_FAILTOPASS_INF_EID, CFE_EVS_INFORMATION,
                                     "AP state change from FAIL to PASS: AP = %d", 
                                      APNumber);
                    }
            }
            /*
            ** Clear consecutive failure counter for this AP
            */
            LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount = 0;
        }
        
        /*
        ** If actionpoint is not measured or has gone stale
        */
        else if (CurrentResult == LC_ACTION_STALE)
        {
            /*
            ** Make sure the consecutive fail count is zeroed
            */
            LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount = 0;
        }
        else
        {
            /*
            ** We got back a LC_ACTION_ERROR result, send event
            */
            CFE_EVS_SendEvent(LC_ACTION_ERROR_ERR_EID, CFE_EVS_ERROR,
                              "AP evaluated to error: AP = %d, Result = %d",
                              APNumber, CurrentResult);
        }
     
    } /* end CurrentAPState if */
    
    return;
    
} /* end LC_SampleSingleAP */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Evaluate the Reverse Polish Notation (RPN) equation for an      */
/* actionpoint                                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */   
#define  POP_RPN_DATA     ((StackPtr <= 0)                       \
                                      ? (IllegalRPN = TRUE)      \
                                      : (RPNStack [--StackPtr]))

#define PUSH_RPN_DATA(x)  ((StackPtr >= LC_MAX_RPN_EQU_SIZE)      \
                                      ? (IllegalRPN = TRUE)       \
                                      : (RPNStack [StackPtr++] = x))
uint8 LC_EvaluateRPN(uint16 APNumber)
{
    boolean  Done;
    boolean  IllegalRPN;
    boolean  IllegalOperand;
    uint8    EvalResult = LC_ACTION_ERROR;
    int32    RPNEquationPtr;
    int32    StackPtr;
    uint16   RPNData;
    uint16   RPNStack [LC_MAX_RPN_EQU_SIZE];
    uint16   Operand1;
    uint16   Operand2;

    StackPtr       = 0;
    RPNEquationPtr = 0;
    IllegalRPN     = FALSE;
    IllegalOperand = FALSE;
    Done           = FALSE;

    /* 
    ** Keep going till we're done or till we get an error 
    */
    while ((Done == FALSE)          && 
           (IllegalRPN == FALSE)    &&
           (IllegalOperand == FALSE))
    {
        /*
        **  Pick off each piece of the RPN equation and handle
        **  one at a time
        */
        RPNData = LC_OperData.ADTPtr[APNumber].RPNEquation[RPNEquationPtr++];

        /*
        **  The data is either an RPN operator or a watchpoint number
        */
        switch (RPNData)
        {
            /*
            **  If the data is an RPN operator then each operand is
            **  either a watchpoint evaluation result or it is the
            **  result of a previous RPN operation
            */
            case LC_RPN_AND:
                Operand2 = POP_RPN_DATA;
                Operand1 = POP_RPN_DATA;
                if ((Operand1 == LC_WATCH_FALSE) ||
                    (Operand2 == LC_WATCH_FALSE))
                {
                    PUSH_RPN_DATA (LC_WATCH_FALSE);
                }
                else if ((Operand1 == LC_WATCH_ERROR) ||
                         (Operand2 == LC_WATCH_ERROR))
                {
                    PUSH_RPN_DATA (LC_WATCH_ERROR);
                }
                else if ((Operand1 == LC_WATCH_STALE) ||
                         (Operand2 == LC_WATCH_STALE))
                {
                    PUSH_RPN_DATA (LC_WATCH_STALE);
                }
                else
                {
                    PUSH_RPN_DATA (LC_WATCH_TRUE);
                }
                break;

            case LC_RPN_OR:
                Operand2 = POP_RPN_DATA;
                Operand1 = POP_RPN_DATA;
                if ((Operand1 == LC_WATCH_TRUE) ||
                    (Operand2 == LC_WATCH_TRUE))
                {
                    PUSH_RPN_DATA (LC_WATCH_TRUE);
                }
                else if ((Operand1 == LC_WATCH_ERROR) ||
                         (Operand2 == LC_WATCH_ERROR))
                {
                    PUSH_RPN_DATA (LC_WATCH_ERROR);
                }
                else if ((Operand1 == LC_WATCH_STALE) ||
                         (Operand2 == LC_WATCH_STALE))
                {
                    PUSH_RPN_DATA (LC_WATCH_STALE);
                }
                else
                {
                    PUSH_RPN_DATA (LC_WATCH_FALSE);
                }
                break;

            case LC_RPN_XOR:
                Operand2 = POP_RPN_DATA;
                Operand1 = POP_RPN_DATA;
                if ((Operand1 == LC_WATCH_ERROR) ||
                    (Operand2 == LC_WATCH_ERROR))
                {
                    PUSH_RPN_DATA (LC_WATCH_ERROR);
                }
                else if ((Operand1 == LC_WATCH_STALE) ||
                         (Operand2 == LC_WATCH_STALE))
                {
                    PUSH_RPN_DATA (LC_WATCH_STALE);
                }
                else
                {
                    PUSH_RPN_DATA (Operand1 != Operand2);
                }
                break;

            case LC_RPN_NOT:
                Operand1 = POP_RPN_DATA;
                if (Operand1 == LC_WATCH_ERROR)
                {
                    PUSH_RPN_DATA (LC_WATCH_ERROR);
                }
                else if (Operand1 == LC_WATCH_STALE)
                {
                    PUSH_RPN_DATA (LC_WATCH_STALE);
                }
                else
                {
                    PUSH_RPN_DATA (Operand1 == LC_WATCH_FALSE);
                }
                break;

            case LC_RPN_EQUAL:
                EvalResult = POP_RPN_DATA;
                if ((EvalResult == LC_WATCH_ERROR) ||
                    (EvalResult == LC_WATCH_STALE))
                {
                    IllegalOperand = TRUE;
                }
                if (StackPtr == 0)
                {
                    Done = TRUE;
                }
                else
                {
                    IllegalRPN = TRUE;
                }
                break;

            /*
            **  If the data is a watchpoint number then the operand is
            **  the current value of that watchpoint result
            */
            default:
                if (RPNData < LC_MAX_WATCHPOINTS)
                {
                    PUSH_RPN_DATA (LC_OperData.WRTPtr[RPNData].WatchResult);
                }
                else
                {
                    IllegalRPN = TRUE;
                }
                break;

        } /* end switch */
        
        /* 
        ** If still not done and have no errors - check for the end of the buffer 
        */
        if ((Done == FALSE)          && 
            (IllegalRPN == FALSE)    && 
            (IllegalOperand == FALSE))
        {
            if (RPNEquationPtr >= LC_MAX_RPN_EQU_SIZE)
            {
                IllegalRPN = TRUE;
            }
        }

    } /* end while */

    if (IllegalRPN == TRUE)
    {
        CFE_EVS_SendEvent(LC_INVALID_RPN_ERR_EID, CFE_EVS_ERROR,
               "AP has illegal RPN expression: AP = %d, LastOperand = %d, StackPtr = %d",
               APNumber, (RPNEquationPtr - 1), StackPtr);
                 
        EvalResult = LC_ACTION_ERROR;
    }
    else if (EvalResult == LC_WATCH_ERROR)
    {
        EvalResult = LC_ACTION_ERROR;
    }
    else if (EvalResult == LC_WATCH_STALE)
    {
        EvalResult = LC_ACTION_STALE;
    }
    else if (EvalResult == LC_WATCH_FALSE)
    {
        EvalResult = LC_ACTION_PASS;
    }
    else if (EvalResult == LC_WATCH_TRUE)
    {
        EvalResult = LC_ACTION_FAIL;
    }

    return (EvalResult);
    
} /* end LC_EvaluateRPN */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Validate the actionpoint definition table (ADT)                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */   
int32 LC_ValidateADT(void *TableData)
{
    LC_ADTEntry_t *TableArray = (LC_ADTEntry_t *) TableData;

    int32 EntryResult = LC_ADTVAL_NO_ERR;
    int32 TableResult = CFE_SUCCESS;
    int32 TableIndex;

    uint8           DefaultState;    
    uint16          RTSId;
    uint16          MaxFailsBeforeRTS;      
    uint16          EventType;
    
    uint16         *RPNPtr; 
     int32          RPNIndex      = 0;
     int32          RPNStackDepth = 0;
    
    int32 GoodCount   = 0;
    int32 BadCount    = 0;
    int32 UnusedCount = 0;

    /*
    ** Verify each entry in the pending actionpoint definition table
    */
    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++ )
    {
        DefaultState      = TableArray[TableIndex].DefaultState;
        RTSId             = TableArray[TableIndex].RTSId;
        MaxFailsBeforeRTS = TableArray[TableIndex].MaxFailsBeforeRTS;
        RPNPtr            = TableArray[TableIndex].RPNEquation;
        EventType         = TableArray[TableIndex].EventType;

        if (DefaultState == LC_ACTION_NOT_USED)
        {
            /*
            ** Unused table entry
            */
            UnusedCount++;
        }
        else if ((DefaultState != LC_APSTATE_ACTIVE)    &&
                 (DefaultState != LC_APSTATE_PASSIVE)   &&
                 (DefaultState != LC_APSTATE_DISABLED)  &&
                 (DefaultState != LC_APSTATE_PERMOFF))
        {
            /*
            ** Invalid default state
            */
            BadCount++;
            EntryResult = LC_ADTVAL_ERR_DEFSTATE;
        }
        else if (RTSId > LC_MAX_VALID_ADT_RTSID)
        {
            /*
            ** Bad RTS ID (limit set by configuration parameter,
            ** see lc_platform_cfg.h)
            */
            BadCount++;
            EntryResult = LC_ADTVAL_ERR_RTSID;
        }
        else if (MaxFailsBeforeRTS == 0)
        {
            /*
            ** Bad fail count
            */
            BadCount++;
            EntryResult = LC_ADTVAL_ERR_FAILCNT;
        }
        else if ((EventType != CFE_EVS_DEBUG)       &&
                 (EventType != CFE_EVS_INFORMATION) &&
                 (EventType != CFE_EVS_ERROR)       &&
                 (EventType != CFE_EVS_CRITICAL))
        {
            /*
            ** Invalid event type
            */
            BadCount++;
            EntryResult = LC_ADTVAL_ERR_EVTTYPE;
        }
        else
        {
            /*
            ** Validate reverse polish equation syntax
            */
            EntryResult = LC_ValidateRPN(RPNPtr, &RPNIndex, &RPNStackDepth);
            
            if (EntryResult != LC_ADTVAL_NO_ERR)
            {
                BadCount++;
            }
            else
            {
                GoodCount++;
            }
        }
        
        /*
        ** Generate detailed event for "first" error
        */
        if ((EntryResult != LC_ADTVAL_NO_ERR) && (TableResult == CFE_SUCCESS))
        {
            if (EntryResult == LC_ADTVAL_ERR_RPN)
            {
                CFE_EVS_SendEvent(LC_ADTVAL_RPNERR_EID, CFE_EVS_ERROR,
                        "ADT verify RPN err: AP = %d, Index = %d, StackDepth = %d",
                        TableIndex, RPNIndex, RPNStackDepth);
            }
            else
            {
                CFE_EVS_SendEvent(LC_ADTVAL_ERR_EID, CFE_EVS_ERROR,
                        "ADT verify err: AP = %d, Err = %d, State = %d, RTS = %d, FailCnt = %d, EvtType = %d",
                        TableIndex, EntryResult, DefaultState, RTSId, MaxFailsBeforeRTS, EventType );
            }
            
            TableResult = EntryResult;
        }
        
    } /* end TableIndex for */
    
    /*
    ** Generate informational event with error totals
    */
    CFE_EVS_SendEvent(LC_ADTVAL_INF_EID, CFE_EVS_INFORMATION,
                     "ADT verify results: good = %d, bad = %d, unused = %d",
                      GoodCount, BadCount, UnusedCount);

    return(TableResult);
    
} /* end LC_ValidateADT */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Validate a reverse polish notation (RPN) equation               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */   
int32 LC_ValidateRPN(uint16 *RPNPtr, 
                     int32  *IndexValue, 
                     int32  *StackDepthValue)
{
    int32   Result = LC_ADTVAL_NO_ERR;
    int32   BufferIndex;
    int32   StackDepth;
    uint16  BufferItem;

    /*
    ** Each equation consists only of watchpoint ID numbers
    ** and polish symbols (and, or, equal, etc).
    **
    ** Each watchpoint ID increases the stack depth and
    ** each polish symbol (except "not" which has no 
    ** effect) decreases the stack depth.
    **
    ** The stack depth must never become negative and the
    ** depth must equal 1 when the "=" symbol is found.
    **
    ** The last symbol in the equation is assumed to be
    ** the "=" symbol, and the test does not examine any
    ** further data.
    */
    StackDepth = 0;

    for (BufferIndex = 0; BufferIndex < LC_MAX_RPN_EQU_SIZE; BufferIndex++)
    {
        BufferItem = RPNPtr[BufferIndex];

        if ((BufferItem == LC_RPN_AND) ||
            (BufferItem == LC_RPN_OR)  ||
            (BufferItem == LC_RPN_XOR))
        {
            /*
            ** Depth test will fail if we haven't already counted 2
            ** watchpoint ID values
            */
            StackDepth--;
        }
        else if (BufferItem == LC_RPN_NOT)
        {
            /*
            ** Depth test will fail if this is 1st symbol
            */
        }
        else if (BufferItem == LC_RPN_EQUAL)
        {
            /*
            ** Equation ends when LC_RPN_EQUAL is found
            */
            break;
        }
        else if (BufferItem < LC_MAX_WATCHPOINTS)
        {
            /*
            ** Valid watchpoint ID
            */
            StackDepth++;
        }
        else
        {
            /*
            ** Not a valid polish symbol or watchpoint ID
            */
            break;
        }

        /*
        ** Must have more watchpoint ID's than polish symbols
        */
        if (StackDepth <= 0)
        {
            break;
        }
    }

    /*
    ** Check for valid Reverse Polish Notation equation
    */
    if ((BufferItem == LC_RPN_EQUAL) && (StackDepth == 1))
    {
        Result = LC_ADTVAL_NO_ERR;
    }
    else
    {
        Result = LC_ADTVAL_ERR_RPN;

        /*
        ** Provide additional error information
        */
        *IndexValue      = BufferIndex;
        *StackDepthValue = StackDepth;
    }

    return(Result);
    
} /* end LC_ValidateRPN */

/************************/
/*  End of File Comment */
/************************/
