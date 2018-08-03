/*
**      Copyright (c) 2004-2012, United States government as represented by the
**      administrator of the National Aeronautics Space Administration.
**      All rights reserved. This software(cFE) was created at NASA's Goddard
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
** File:
**    sb_UT.h
**
** Purpose:
**    SB unit test header
**
** References:
**    1. cFE Application Developers Guide
**    2. unit test standard 092503
**    3. C Coding Standard 102904
**
** Notes:
**    1. This is unit test code only, not for use in flight
**
** $Date: 2014/05/28 09:23:30GMT-05:00 $
** $Revision: 1.1 $
**
*/
#ifndef _sb_UT_h_
#define _sb_UT_h_

/*
** Includes
*/
#include <string.h>
#include "cfe.h"
#include "cfe_sb_events.h"
#include "cfe_sb_priv.h"
#include "osapi.h"
#include "common_types.h"
#include "ut_stubs.h"

/*
** Macro definitions
*/
#define CFE_FAIL 0
#define CFE_PASS 1

/*
** Structures
*/
typedef struct {
     CFE_SB_CmdHdr_t Hdr; /* 8 bytes */
     uint32          Cmd32Param1;
     uint16          Cmd16Param1;
     uint16          Cmd16Param2;
     uint8           Cmd8Param1;
     uint8           Cmd8Param2;
     uint8           Cmd8Param3;
     uint8           Cmd8Param4;
} SB_UT_Test_Cmd_t;

typedef struct {
     CFE_SB_TlmHdr_t Hdr; /* 12 bytes */
     uint32          Tlm32Param1;
     uint16          Tlm16Param1;
     uint16          Tlm16Param2;
     uint8           Tlm8Param1;
     uint8           Tlm8Param2;
     uint8           Tlm8Param3;
     uint8           Tlm8Param4;
} SB_UT_Test_Tlm_t;

typedef struct {
     CFE_SB_Msg_t Pri; /* 6 bytes */
     uint8        Tlm8Param1;
     uint8        Tlm8Param2;
     uint32       Tlm32Param1;
     uint16       Tlm16Param1;
     uint16       Tlm16Param2;
} SB_UT_TstPktWoSecHdr_t;

/*
** External global variables
*/
extern char cMsg[];

/* SB unit test functions */
/*****************************************************************************/
/**
** \brief Initialize settings in preparation for the next unit test
**
** \par Description
**        This function initializes settings in preparation for the next unit
**        test.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_CheckForOpenSockets, #UT_InitData, #CFE_SB_EarlyInit
**
******************************************************************************/
void SB_ResetUnitTest(void);

/*****************************************************************************/
/**
** \brief Function for calling SB application test functions
**
** \par Description
**        Function for calling SB application test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #Test_SB_AppInit, #Test_SB_MainRoutine, #Test_SB_Cmds
**
******************************************************************************/
void Test_SB_App(void);

/*****************************************************************************/
/**
** \brief Function for calling SB application initialization
**        test functions
**
** \par Description
**        Function for calling SB application initialization
**        test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #Test_SB_AppInit_ESRegFail(), #Test_SB_AppInit_EVSRegFail(),
** \sa #Test_SB_AppInit_EVSSendEvtFail(), #Test_SB_AppInit_CrPipeFail(),
** \sa #Test_SB_AppInit_Sub1Fail(), #Test_SB_AppInit_Sub2Fail(),
** \sa #Test_SB_AppInit_GetPoolFail(), #Test_SB_AppInit_PutPoolFail()
**
******************************************************************************/
void Test_SB_AppInit(void);

/*****************************************************************************/
/**
** \brief Test task init with ES_RegisterApp returning error
**
** \par Description
**        This function tests task init with ES_RegisterApp returning error.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #UT_SetRtnCode, #CFE_SB_AppInit,
** \sa #UT_GetNumEventsSent, #UT_Report
**
******************************************************************************/
void Test_SB_AppInit_ESRegFail(void);

/*****************************************************************************/
/**
** \brief Test task init with EVS_Register returning error
**
** \par Description
**        This function tests task init with EVS_Register returning error.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #UT_SetRtnCode, #CFE_SB_AppInit,
** \sa #UT_GetNumEventsSent, #UT_Report
**
******************************************************************************/
void Test_SB_AppInit_EVSRegFail(void);

/*****************************************************************************/
/**
** \brief Test task init with EVS_SendEvent returning error on task init event
**
** \par Description
**        This function tests task init with EVS_SendEvent returning error on
**        task init event.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #UT_SetRtnCode, #CFE_SB_AppInit,
** \sa #UT_GetNumEventsSent, #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_SB_AppInit_EVSSendEvtFail(void);

/*****************************************************************************/
/**
** \brief Test task init with pipe create failure
**
** \par Description
**        This function tests task init with pipe create failure.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_AppInit, #UT_GetNumEventsSent,
** \sa #UT_EventIsInHistory, #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_SB_AppInit_CrPipeFail(void);

/*****************************************************************************/
/**
** \brief Test task init with a failure on first subscription request
**
** \par Description
**        This function tests task init with a failure on first subscription
**        request.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #UT_SetRtnCode, #CFE_SB_AppInit,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #CFE_SB_DeletePipe,
** \sa #UT_Report
**
******************************************************************************/
void Test_SB_AppInit_Sub1Fail(void);

/*****************************************************************************/
/**
** \brief Test task init with a failure on second subscription request
**
** \par Description
**        This function tests task init with a failure on second subscription
**        request.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #UT_SetRtnCode, #CFE_SB_AppInit,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #CFE_SB_DeletePipe,
** \sa #UT_Report
**
******************************************************************************/
void Test_SB_AppInit_Sub2Fail(void);

/*****************************************************************************/
/**
** \brief Test task init with a GetPool failure
**
** \par Description
**        This function tests task init with a GetPool failure.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #UT_SetRtnCode, #CFE_SB_AppInit,
** \sa #UT_GetNumEventsSent, #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_SB_AppInit_GetPoolFail(void);

/*****************************************************************************/
/**
** \brief Test task init with a PutPool failure
**
** \par Description
**        This function tests task init with a PutPool failure.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #UT_SetRtnCode, #CFE_SB_AppInit,
** \sa #UT_GetNumEventsSent, #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_SB_AppInit_PutPoolFail(void);

/*****************************************************************************/
/**
** \brief Function for calling SB main task test functions
**
** \par Description
**        Function for calling SB main task test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #Test_SB_Main_RcvErr, #Test_SB_Main_InitErr
**
******************************************************************************/
void Test_SB_MainRoutine(void);

/*****************************************************************************/
/**
** \brief Test main task with a packet receive error
**
** \par Description
**        This function tests main task with a packet receive error.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #UT_SetRtnCode, #CFE_SB_TaskMain,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #CFE_SB_DeletePipe,
** \sa #UT_Report
**
******************************************************************************/
void Test_SB_Main_RcvErr(void);

/*****************************************************************************/
/**
** \brief Test main task with an application initialization error
**
** \par Description
**        This function tests main task with an application initialization
**        error.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #UT_SetRtnCode, #CFE_SB_TaskMain,
** \sa #UT_GetNumEventsSent, #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_SB_Main_InitErr(void);

/*****************************************************************************/
/**
** \brief Function for calling SB command test functions
**
** \par Description
**        Function for calling SB command test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #Test_SB_Cmds_Noop(), #Test_SB_Cmds_RstCtrs(),
** \sa #Test_SB_Cmds_Stats(), #Test_SB_Cmds_RoutingInfoDef(),
** \sa #Test_SB_Cmds_RoutingInfoSpec(), #Test_SB_Cmds_RoutingInfoCreateFail(),
** \sa #Test_SB_Cmds_RoutingInfoHdrFail(),
** \sa #Test_SB_Cmds_RoutingInfoWriteFail(), #Test_SB_Cmds_PipeInfoDef(),
** \sa #Test_SB_Cmds_PipeInfoSpec(), #Test_SB_Cmds_PipeInfoCreateFail(),
** \sa #Test_SB_Cmds_PipeInfoHdrFail(), #Test_SB_Cmds_PipeInfoWriteFail(),
** \sa #Test_SB_Cmds_MapInfoDef(), #Test_SB_Cmds_MapInfoSpec(),
** \sa #Test_SB_Cmds_MapInfoCreateFail(), #Test_SB_Cmds_MapInfoHdrFail(),
** \sa #Test_SB_Cmds_MapInfoWriteFail(), #Test_SB_Cmds_EnRouteValParam(),
** \sa #Test_SB_Cmds_EnRouteNonExist(), #Test_SB_Cmds_EnRouteInvParam(),
** \sa #Test_SB_Cmds_EnRouteInvParam2(), #Test_SB_Cmds_EnRouteInvParam3(),
** \sa #Test_SB_Cmds_DisRouteValParam(), #Test_SB_Cmds_DisRouteNonExist(),
** \sa #Test_SB_Cmds_DisRouteInvParam(), #Test_SB_Cmds_DisRouteInvParam2(),
** \sa #Test_SB_Cmds_DisRouteInvParam3(), #Test_SB_Cmds_SendHK(),
** \sa #Test_SB_Cmds_SendPrevSubs(), #Test_SB_Cmds_SubRptOn(),
** \sa #Test_SB_Cmds_SubRptOff(), #Test_SB_Cmds_UnexpCmdCode(),
** \sa #Test_SB_Cmds_UnexpMsgId()
**
******************************************************************************/
void Test_SB_Cmds(void);

/*****************************************************************************/
/**
** \brief Test no-op command
**
** \par Description
**        This function tests the no-op command.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_SetCmdCode,
** \sa #CFE_SB_ProcessCmdPipePkt, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_Noop(void);

/*****************************************************************************/
/**
** \brief Test reset counters command
**
** \par Description
**        This function tests the reset counters command.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_SetCmdCode,
** \sa #CFE_SB_ProcessCmdPipePkt, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_RstCtrs(void);

/*****************************************************************************/
/**
** \brief Test send SB stats command
**
** \par Description
**        This function tests the end SB stats command.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_SetCmdCode,
** \sa #CFE_SB_ProcessCmdPipePkt, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_Stats(void);

/*****************************************************************************/
/**
** \brief Test send routing information command using the default file name
**
** \par Description
**        This function tests the send routing information command using the
**        default file name.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_SetCmdCode,
** \sa #CFE_SB_AppInit, #CFE_SB_ProcessCmdPipePkt, #UT_GetNumEventsSent,
** \sa #UT_EventIsInHistory, #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_RoutingInfoDef(void);

/*****************************************************************************/
/**
** \brief Test send routing information command using a specified file name
**
** \par Description
**        This function tests the send routing information command using a
**        specified file name.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_SetCmdCode,
** \sa #CFE_SB_ProcessCmdPipePkt, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_RoutingInfoSpec(void);

/*****************************************************************************/
/**
** \brief Test send routing information command with a file creation failure
**
** \par Description
**        This function tests the send routing information command with a file
**        creation failure.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_SetCmdCode,
** \sa #UT_SetOSFail, #CFE_SB_ProcessCmdPipePkt, #UT_GetNumEventsSent,
** \sa #UT_EventIsInHistory, #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_RoutingInfoCreateFail(void);

/*****************************************************************************/
/**
** \brief Test send routing information command with a file header
**        write failure
**
** \par Description
**        This function tests the send routing information command with a file
**        header write failure.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #UT_SetRtnCode, #CFE_SB_SendRtgInfo,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_RoutingInfoHdrFail(void);

/*****************************************************************************/
/**
** \brief Test send routing information command with a file header write
**        failure on the second write
**
** \par Description
**        This function tests the send routing information command with a file
**        header write failure on the second write.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_AppInit, #UT_SetRtnCode,
** \sa #CFE_SB_SendRtgInfo, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_RoutingInfoWriteFail(void);

/*****************************************************************************/
/**
** \brief Test send pipe information command using the default file name
**
** \par Description
**        This function tests the send pipe information command using the
**        default file name.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_SetCmdCode,
** \sa #CFE_SB_CreatePipe, #CFE_SB_ProcessCmdPipePkt, #UT_GetNumEventsSent,
** \sa #UT_EventIsInHistory, #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_PipeInfoDef(void);

/*****************************************************************************/
/**
** \brief Test send pipe information command using a specified file name
**
** \par Description
**        This function tests the send pipe information command using a
**        specified file name.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_SetCmdCode,
** \sa #CFE_SB_ProcessCmdPipePkt, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_PipeInfoSpec(void);

/*****************************************************************************/
/**
** \brief Test send pipe information command with a file creation failure
**
** \par Description
**        This function tests the send pipe information command with a file
**        creation failure.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #UT_SetOSFail, #CFE_SB_SendPipeInfo,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_PipeInfoCreateFail(void);

/*****************************************************************************/
/**
** \brief Test send pipe information command with a file header
**        write failure
**
** \par Description
**        This function tests the send pipe information command with a file
**        header write failure.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #UT_SetRtnCode, #CFE_SB_SendPipeInfo,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_PipeInfoHdrFail(void);

/*****************************************************************************/
/**
** \brief Test send pipe information command with a file write failure on
**        the second write
**
** \par Description
**        This function tests the send pipe information command with a file
**        write failure on the second write.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #UT_SetRtnCode,
** \sa #CFE_SB_SendPipeInfo, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_PipeInfoWriteFail(void);

/*****************************************************************************/
/**
** \brief Test send map information command using the default file name
**
** \par Description
**        This function tests the send map information command using the
**        default file name.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_SetCmdCode,
** \sa #CFE_SB_CreatePipe, #CFE_SB_Subscribe, #CFE_SB_ProcessCmdPipePkt,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #CFE_SB_DeletePipe,
** \sa #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_MapInfoDef(void);

/*****************************************************************************/
/**
** \brief Test send map information command using a specified file name
**
** \par Description
**        This function tests the send map information command using a
**        specified file name.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_SetCmdCode,
** \sa #CFE_SB_ProcessCmdPipePkt, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_MapInfoSpec(void);

/*****************************************************************************/
/**
** \brief Test send map information command with a file creation failure
**
** \par Description
**        This function tests the send map information command with a file
**        creation failure.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #UT_SetOSFail, #CFE_SB_SendMapInfo,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_MapInfoCreateFail(void);

/*****************************************************************************/
/**
** \brief Test send map information command with a file header write failure
**
** \par Description
**        This function tests the send map information command with a file
**        header write failure.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #UT_SetRtnCode, #CFE_SB_SendMapInfo,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_MapInfoHdrFail(void);

/*****************************************************************************/
/**
** \brief Test send map information command with a file write failure on
**        the second write
**
** \par Description
**        This function tests the send map information command with a file
**        write failure on the second write.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_Subscribe,
** \sa #UT_SetRtnCode, #CFE_SB_SendMapInfo, #UT_GetNumEventsSent,
** \sa #UT_EventIsInHistory, #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_MapInfoWriteFail(void);

/*****************************************************************************/
/**
** \brief Test command to enable a specific route using a valid route
**
** \par Description
**        This function tests the command to enable a specific route using a
**        valid route.  The requested pipe is defined and subscribed.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_Subscribe,
** \sa #CFE_SB_InitMsg, #CFE_SB_SetCmdCode, #CFE_SB_ProcessCmdPipePkt,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #CFE_SB_DeletePipe,
** \sa #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_EnRouteValParam(void);

/*****************************************************************************/
/**
** \brief Test command to enable a specific route using a non-existent route
**
** \par Description
**        This function tests the command to enable a specific route using a
**        non-existent route.  The requested pipe is defined but not
**        subscribed.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_Subscribe,
** \sa #CFE_SB_InitMsg, #CFE_SB_SetCmdCode, #CFE_SB_ProcessCmdPipePkt,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #CFE_SB_DeletePipe,
** \sa #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_EnRouteNonExist(void);

/*****************************************************************************/
/**
** \brief Test command to enable a specific route using an invalid pipe ID
**
** \par Description
**        This function tests the command to enable a specific route using an
**        invalid pipe ID (the requested pipe is undefined).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_SetCmdCode,
** \sa #CFE_SB_ProcessCmdPipePkt, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_EnRouteInvParam(void);

/*****************************************************************************/
/**
** \brief Test command to enable a specific route using an invalid message ID
**
** \par Description
**        This function tests the command to enable a specific route using an
**        invalid message ID (= CFE_SB_INVALID_MSG_ID).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_SetCmdCode,
** \sa #CFE_SB_ProcessCmdPipePkt, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_EnRouteInvParam2(void);

/*****************************************************************************/
/**
** \brief Test command to enable a specific route using a message ID greater
**        than the maximum allowed
**
** \par Description
**        This function tests the command to enable a specific route using a
**        message ID greater than the maximum allowed
**        (CFE_SB_HIGHEST_VALID_MSGID).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_SetCmdCode,
** \sa #CFE_SB_ProcessCmdPipePkt, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_EnRouteInvParam3(void);

/*****************************************************************************/
/**
** \brief Test command to disable a specific route using a valid route
**
** \par Description
**        This function tests the command to disable a specific route using a
**        valid route.  The requested pipe is defined and subscribed.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_Subscribe,
** \sa #CFE_SB_InitMsg, #CFE_SB_SetCmdCode, #CFE_SB_ProcessCmdPipePkt,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #CFE_SB_DeletePipe,
** \sa #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_DisRouteValParam(void);

/*****************************************************************************/
/**
** \brief Test command to disable a specific route using an invalid pipe ID
**
** \par Description
**        This function tests the command to disable a specific route using an
**        invalid pipe ID.  The requested pipe is undefined.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_Subscribe,
** \sa #CFE_SB_InitMsg, #CFE_SB_SetCmdCode, #CFE_SB_ProcessCmdPipePkt,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #CFE_SB_DeletePipe,
** \sa #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_DisRouteNonExist(void);

/*****************************************************************************/
/**
** \brief Test command to disable a specific route using an invalid pipe ID
**
** \par Description
**        This function tests the command to disable a specific route using an
**        invalid pipe ID.  The requested pipe is undefined.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_SetCmdCode,
** \sa #CFE_SB_ProcessCmdPipePkt, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_DisRouteInvParam(void);

/*****************************************************************************/
/**
** \brief Test command to disable a specific route using an invalid message ID
**
** \par Description
**        This function tests the command to disable a specific route using an
**        invalid message ID (= CFE_SB_INVALID_MSG_ID).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_SetCmdCode,
** \sa #CFE_SB_ProcessCmdPipePkt, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_DisRouteInvParam2(void);

/*****************************************************************************/
/**
** \brief Test command to disable a specific route using a message ID greater
**        than the maximum allowed
**
** \par Description
**        This function tests the command to disable a specific route using a
**        message ID greater than the maximum allowed
**        (CFE_SB_HIGHEST_VALID_MSGID).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_SetCmdCode,
** \sa #CFE_SB_ProcessCmdPipePkt, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_DisRouteInvParam3(void);

/*****************************************************************************/
/**
** \brief Test send housekeeping information command
**
** \par Description
**        This function tests the send housekeeping information command.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_ProcessCmdPipePkt,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_SendHK(void);

/*****************************************************************************/
/**
** \brief Test command to build and send a SB packet containing a complete
**        list of current subscriptions
**
** \par Description
**        This function tests the command to build and send a SB packet
**        containing a complete list of current subscriptions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_SetCmdCode,
** \sa #CFE_SB_CreatePipe, #CFE_SB_Subscribe, #CFE_SB_SubscribeLocal,
** \sa #CFE_SB_ProcessCmdPipePkt, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_SendPrevSubs(void);

/*****************************************************************************/
/**
** \brief Test command to enable subscription reporting
**
** \par Description
**        This function tests the command to enable subscription reporting.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_SetCmdCode,
** \sa #CFE_SB_ProcessCmdPipePkt, #UT_GetNumEventsSent, #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_SubRptOn(void);

/*****************************************************************************/
/**
** \brief Test command to disable subscription reporting
**
** \par Description
**        This function tests the command to disable subscription reporting.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_SetCmdCode,
** \sa #CFE_SB_ProcessCmdPipePkt, #UT_GetNumEventsSent, #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_SubRptOff(void);

/*****************************************************************************/
/**
** \brief Test command handler response to an invalid command code
**
** \par Description
**        This function tests the command handler response to an invalid
**        command code.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_SetCmdCode,
** \sa #CFE_SB_ProcessCmdPipePkt, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_UnexpCmdCode(void);

/*****************************************************************************/
/**
** \brief Test command handler response to an invalid message ID
**
** \par Description
**        This function tests the command handler response to an invalid
**        message ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_ProcessCmdPipePkt,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #UT_Report
**
******************************************************************************/
void Test_SB_Cmds_UnexpMsgId(void);

/*****************************************************************************/
/**
** \brief Function for calling SB early initialization, API, and utility
**        test functions
**
** \par Description
**        Function for calling SB early initialization, API, and utility
**        test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #Test_SB_EarlyInit, #Test_SB_APIs, #Test_SB_Utils
**
******************************************************************************/
void Test_SB_Lib(void);

/*****************************************************************************/
/**
** \brief Function for calling early initialization tests
**
** \par Description
**        Function for calling early initialization tests.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #Test_SB_EarlyInit_SemCreateError,
** \sa #Test_SB_EarlyInit_PoolCreateError, #Test_SB_EarlyInit_NoErrors
**
******************************************************************************/
void Test_SB_EarlyInit(void);

/*****************************************************************************/
/**
** \brief Test early initialization response to a semaphore create failure
**
** \par Description
**        This function tests the early initialization response to a semaphore
**        create failure.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #UT_SetRtnCode, #UT_Report
**
******************************************************************************/
void Test_SB_EarlyInit_SemCreateError(void);

/*****************************************************************************/
/**
** \brief Test early initialization response to a pool create ex failure
**
** \par Description
**        This function tests the early initialization response to a pool
**        create ex failure.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #UT_SetRtnCode, #UT_Report
**
******************************************************************************/
void Test_SB_EarlyInit_PoolCreateError(void);

/*****************************************************************************/
/**
** \brief Test successful early initialization
**
** \par Description
**        This function tests successful early initialization.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_EarlyInit, #UT_Report
**
******************************************************************************/
void Test_SB_EarlyInit_NoErrors(void);

/*****************************************************************************/
/**
** \brief Function for calling SB API test functions
**
** \par Description
**        Function for calling SB API test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #Test_CreatePipe_API, #Test_DeletePipe_API, #Test_Subscribe_API,
** \sa #Test_Unsubscribe_API, #Test_SendMsg_API, #Test_RcvMsg_API,
** \sa #Test_CleanupApp_API
**
******************************************************************************/
void Test_SB_APIs(void);

/*****************************************************************************/
/**
** \brief Function for calling SB create pipe API test functions
**
** \par Description
**        TFunction for calling SB create pipe API test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #Test_CreatePipe_NullPtr, #Test_CreatePipe_ValPipeDepth,
** \sa #Test_CreatePipe_InvalPipeDepth, #Test_CreatePipe_EmptyPipeName,
** \sa #Test_CreatePipe_LongPipeName, #Test_CreatePipe_SamePipeName,
** \sa #Test_CreatePipe_MaxPipes
**
******************************************************************************/
void Test_CreatePipe_API(void);

/*****************************************************************************/
/**
** \brief Test create pipe response to a null pipe ID pointer
**
** \par Description
**        This function tests the create pipe response to a null pipe ID
**        pointer.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #UT_SetRtnCode, #CFE_SB_CreatePipe,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #UT_Report
**
******************************************************************************/
void Test_CreatePipe_NullPtr(void);

/*****************************************************************************/
/**
** \brief Test create pipe response to valid pipe depths
**
** \par Description
**        This function tests the create pipe response to valid pipe depths.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #UT_SetRtnCode, #CFE_SB_CreatePipe,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #UT_Report
**
******************************************************************************/
void Test_CreatePipe_ValPipeDepth(void);

/*****************************************************************************/
/**
** \brief Test create pipe response to invalid pipe depths
**
** \par Description
**        This function tests the create pipe response to invalid pipe depths.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #UT_SetRtnCode, #CFE_SB_CreatePipe,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #UT_Report
**
******************************************************************************/
void Test_CreatePipe_InvalPipeDepth(void);

/*****************************************************************************/
/**
** \brief Test create pipe response to an empty pipe name
**
** \par Description
**        This function tests the create pipe response to an empty pipe name.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #UT_GetNumEventsSent,
** \sa #UT_EventIsInHistory, #UT_Report
**
******************************************************************************/
void Test_CreatePipe_EmptyPipeName(void);

/*****************************************************************************/
/**
** \brief Test create pipe response to a pipe name longer than allowed
**
** \par Description
**        This function tests the create pipe response to a pipe name longer
**        than allowed.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #UT_GetNumEventsSent,
** \sa #UT_EventIsInHistory, #UT_Report
**
******************************************************************************/
void Test_CreatePipe_LongPipeName(void);

/*****************************************************************************/
/**
** \brief Test create pipe response to duplicate pipe names
**
** \par Description
**        This function tests the create pipe response to duplicate pipe names.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #UT_GetNumEventsSent,
** \sa #UT_EventIsInHistory, #UT_Report
**
******************************************************************************/
void Test_CreatePipe_SamePipeName(void);

/*****************************************************************************/
/**
** \brief Test create pipe response to too many pipes
**
** \par Description
**        This function tests the create pipe response to too many pipes.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #UT_GetNumEventsSent,
** \sa #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_CreatePipe_MaxPipes(void);

/*****************************************************************************/
/**
** \brief Function for calling SB delete pipe API test functions
**
** \par Description
**        Function for calling SB delete pipe API test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #Test_DeletePipe_NoSubs, #Test_DeletePipe_WithSubs,
** \sa #Test_DeletePipe_InvalidPipeId, #Test_DeletePipe_InvalidPipeOwner,
** \sa #Test_DeletePipe_WithAppid
**
******************************************************************************/
void Test_DeletePipe_API(void);

/*****************************************************************************/
/**
** \brief Test delete pipe response to deleting a pipe with no subscriptions
**
** \par Description
**        This function tests the delete pipe response to deleting a pipe with
**        no subscriptions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_DeletePipe, #UT_GetNumEventsSent,
** \sa #UT_EventIsInHistory, #UT_Report
**
******************************************************************************/
void Test_DeletePipe_NoSubs(void);

/*****************************************************************************/
/**
** \brief Test delete pipe response to deleting a pipe with subscriptions
**
** \par Description
**        This function tests the delete pipe response to deleting a pipe with
**        subscriptions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_DeletePipe, #UT_GetNumEventsSent,
** \sa #UT_EventIsInHistory, #UT_Report
**
******************************************************************************/
void Test_DeletePipe_WithSubs(void);

/*****************************************************************************/
/**
** \brief Test delete pipe response to an invalid pipe ID
**
** \par Description
**        This function tests the delete pipe response to an invalid pipe ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_DeletePipe, #UT_GetNumEventsSent,
** \sa #UT_EventIsInHistory, #UT_Report
**
******************************************************************************/
void Test_DeletePipe_InvalidPipeId(void);

/*****************************************************************************/
/**
** \brief Test delete pipe response to an invalid pipe owner
**
** \par Description
**        This function tests the delete pipe response to an invalid pipe
**        owner.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, CFE_SB_CreatePipe, #CFE_SB_DeletePipe,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #UT_Report
**
******************************************************************************/
void Test_DeletePipe_InvalidPipeOwner(void);

/*****************************************************************************/
/**
** \brief Test successful pipe delete with app ID
**
** \par Description
**        This function tests successful pipe delete with app ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_Subscribe,
** \sa #CFE_SB_DeletePipeWithAppId, #UT_GetNumEventsSent, #UT_Report
**
******************************************************************************/
void Test_DeletePipe_WithAppid(void);

/*****************************************************************************/
/**
** \brief Function for calling SB subscribe API test functions
**
** \par Description
**        Function for calling SB subscribe API test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #Test_Subscribe_SubscribeEx, #Test_Subscribe_InvalidPipeId,
** \sa #Test_Subscribe_InvalidMsgId, #Test_Subscribe_MaxMsgLim,
** \sa #Test_Subscribe_DuplicateSubscription,
** \sa #Test_Subscribe_LocalSubscription, #Test_Subscribe_MaxDestCount,
** \sa #Test_Subscribe_MaxMsgIdCount, #Test_Subscribe_SendPrevSubs,
** \sa #Test_Subscribe_FindGlobalMsgIdCnt, #Test_Subscribe_PipeNonexistent,
** \sa #Test_Subscribe_SubscriptionReporting, #Test_Subscribe_InvalidPipeOwner
**
******************************************************************************/
void Test_Subscribe_API(void);

/*****************************************************************************/
/**
** \brief Test API to globally subscribe to a message
**
** \par Description
**        This function tests globally subscribing to a message.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_SubscribeEx,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #CFE_SB_DeletePipe,
** \sa #UT_Report
**
******************************************************************************/
void Test_Subscribe_SubscribeEx(void);

/*****************************************************************************/
/**
** \brief Test message subscription response to an invalid pipe ID
**
** \par Description
**        This function tests the message subscription response to an invalid
**        pipe ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_Subscribe, #UT_GetNumEventsSent,
** \sa #UT_EventIsInHistory, #UT_Report
**
******************************************************************************/
void Test_Subscribe_InvalidPipeId(void);

/*****************************************************************************/
/**
** \brief Test message subscription response to an invalid message ID
**
** \par Description
**        This function tests the message subscription response to an invalid
**        message ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_Subscribe,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #CFE_SB_DeletePipe,
** \sa #UT_Report
**
******************************************************************************/
void Test_Subscribe_InvalidMsgId(void);

/*****************************************************************************/
/**
** \brief Test message subscription response to the maximum message limit
**
** \par Description
**        This function tests the message subscription response to the maximum
**        message limit.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_SubscribeEx,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #CFE_SB_DeletePipe,
** \sa #UT_Report
**
******************************************************************************/
void Test_Subscribe_MaxMsgLim(void);

/*****************************************************************************/
/**
** \brief Test message subscription response to a duplicate subscription
**
** \par Description
**        This function tests the message subscription response to a duplicate
**        subscription.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_Subscribe,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #CFE_SB_DeletePipe,
** \sa #UT_Report
**
******************************************************************************/
void Test_Subscribe_DuplicateSubscription(void);

/*****************************************************************************/
/**
** \brief Test API to locally subscribe to a message
**
** \par Description
**        This function tests locally subscribing to a message.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_SubscribeLocal,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #CFE_SB_DeletePipe,
** \sa #UT_Report
**
******************************************************************************/
void Test_Subscribe_LocalSubscription(void);

/*****************************************************************************/
/**
** \brief Test message subscription response to reaching the maximum
**        destination count
**
** \par Description
**        This function tests the message subscription response to reaching the
**        maximum destination count.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_Subscribe,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #CFE_SB_DeletePipe,
** \sa #UT_Report
**
******************************************************************************/
void Test_Subscribe_MaxDestCount(void);

/*****************************************************************************/
/**
** \brief Test message subscription response to reaching the maximum
**        message ID count
**
** \par Description
**        This function tests the message subscription response to reaching the
**        maximum message ID count.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_Subscribe,
** \sa #UT_EventIsInHistory, #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_Subscribe_MaxMsgIdCount(void);

/*****************************************************************************/
/**
** \brief Test obtaining the list of current message subscriptions
**
** \par Description
**        This function tests obtaining the list of current message
**        subscriptions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_Subscribe,
** \sa #CFE_SB_SendPrevSubs, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_Subscribe_SendPrevSubs(void);

/*****************************************************************************/
/**
** \brief Test function to get a count of the global message IDs in use
**
** \par Description
**        This function tests the function to get a count of the global message
**        IDs in use.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_Subscribe,
** \sa #CFE_SB_SubscribeLocal, #CFE_SB_FindGlobalMsgIdCnt,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #CFE_SB_DeletePipe,
** \sa #UT_Report
**
******************************************************************************/
void Test_Subscribe_FindGlobalMsgIdCnt(void);

/*****************************************************************************/
/**
** \brief Test message subscription response to nonexistent pipe
**
** \par Description
**        This function tests the message subscription response to nonexistent
**        pipe.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_Subscribe, UT_GetNumEventsSent,
** \sa #UT_EventIsInHistory, #UT_Report
**
******************************************************************************/
void Test_Subscribe_PipeNonexistent(void);

/*****************************************************************************/
/**
** \brief Test enabling and disabling subscription reporting
**
** \par Description
**        This function tests enabling and disabling subscription reporting.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe,
** \sa #CFE_SB_SetSubscriptionReporting, #CFE_SB_Subscribe,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #CFE_SB_DeletePipe,
** \sa UT_Report
**
******************************************************************************/
void Test_Subscribe_SubscriptionReporting(void);

/*****************************************************************************/
/**
** \brief Test message subscription response to an invalid pipe owner
**
** \par Description
**        This function tests the message subscription response to an invalid
**        pipe owner.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_Subscribe,
** \sa #UT_EventIsInHistory, #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_Subscribe_InvalidPipeOwner(void);

/*****************************************************************************/
/**
** \brief Function for calling SB unsubscribe API test functions
**
** \par Description
**        Function for calling SB unsubscribe API test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #Test_Unsubscribe_Basic, #Test_Unsubscribe_Local,
** \sa #Test_Unsubscribe_InvalParam, #Test_Unsubscribe_NoMatch,
** \sa #Test_Unsubscribe_SubscriptionReporting, #Test_Unsubscribe_InvalidPipe,
** \sa #Test_Unsubscribe_InvalidPipeOwner, #Test_Unsubscribe_FirstDestWithMany,
** \sa #Test_Unsubscribe_MiddleDestWithMany, #Test_Unsubscribe_GetDestPtr
**
******************************************************************************/
void Test_Unsubscribe_API(void);

/*****************************************************************************/
/**
** \brief Test API used to unsubscribe to a message (successful)
**
** \par Description
**        This function tests unsubscribing to a message (successful).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_Subscribe,
** \sa #CFE_SB_Unsubscribe, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_Unsubscribe_Basic(void);

/*****************************************************************************/
/**
** \brief Test CFE internal API used to locally unsubscribe to a message
**        (successful)
**
** \par Description
**        This function tests locally unsubscribing to a message
**        (successful).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_Subscribe,
** \sa #CFE_SB_UnsubscribeLocal, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_Unsubscribe_Local(void);

/*****************************************************************************/
/**
** \brief Test message unsubscription response to an invalid message ID
**
** \par Description
**        This function tests the message unsubscription response to an invalid
**        message ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_UnSubscribe,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #CFE_SB_DeletePipe,
** \sa #UT_Report
**
******************************************************************************/
void Test_Unsubscribe_InvalParam(void);

/*****************************************************************************/
/**
** \brief Test message unsubscription response to a message ID that is
**        not subscribed
**
** \par Description
**        This function tests the message unsubscription response to a message
**        ID that is not subscribed.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_Subscribe,
** \sa #CFE_SB_Unsubscribe, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_Unsubscribe_NoMatch(void);

/*****************************************************************************/
/**
** \brief Test message unsubscription response to enabling/disabling
**        subscription reporting
**
** \par Description
**        This function tests the message unsubscription response to enabling/
**        disabling subscription reporting.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_Subscribe,
** \sa #CFE_SB_SetSubscriptionReporting, #CFE_SB_Unsubscribe,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #CFE_SB_DeletePipe,
** \sa #UT_Report
**
******************************************************************************/
void Test_Unsubscribe_SubscriptionReporting(void);

/*****************************************************************************/
/**
** \brief Test message unsubscription response to an invalid pipe ID
**
** \par Description
**        This function tests the message unsubscription response to an invalid
**        pipe ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_Subscribe,
** \sa #CFE_SB_Unsubscribe, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_Unsubscribe_InvalidPipe(void);

/*****************************************************************************/
/**
** \brief Test message unsubscription response to an invalid pipe owner
**
** \par Description
**        This function tests the message unsubscription response to an invalid
**        pipe owner.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_Subscribe,
** \sa #CFE_SB_Unsubscribe, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_Unsubscribe_InvalidPipeOwner(void);

/*****************************************************************************/
/**
** \brief Test message unsubscription response to the first pipe destination
**        when the message is subscribed to by multiple pipes
**
** \par Description
**        This function tests the message unsubscription response to the first
**        pipe destination when the message is subscribed to by multiple pipes.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_Subscribe,
** \sa #CFE_SB_Unsubscribe, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_Unsubscribe_FirstDestWithMany(void);

/*****************************************************************************/
/**
** \brief Test message unsubscription response to a middle pipe destination
**        when the message is subscribed to by multiple pipes
**
** \par Description
**        This function tests the message unsubscription response to a middle
**        pipe destination when the message is subscribed to by multiple pipes.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_Subscribe,
** \sa #CFE_SB_Unsubscribe, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_Unsubscribe_MiddleDestWithMany(void);

/*****************************************************************************/
/**
** \brief  Test message unsubscription by verifying the message destination
**         pointer no longer points to the pipe
**
** \par Description
**        This function tests message unsubscription by verifying the message
**        destination pointer no longer points to the pipe.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_Subscribe,
** \sa #CFE_SB_Unsubscribe, #CFE_SB_GetDestPtr, #UT_GetNumEventsSent,
** \sa #UT_EventIsInHistory, #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_Unsubscribe_GetDestPtr(void);

/*****************************************************************************/
/**
** \brief Function for calling SB send message API test functions
**
** \par Description
**        Function for calling SB send message API test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #Test_SendMsg_NullPtr, #Test_SendMsg_InvalidMsgId,
** \sa #Test_SendMsg_NoSubscribers, #Test_SendMsg_MaxMsgSizePlusOne,
** \sa #Test_SendMsg_BasicSend, #Test_SendMsg_SequenceCount,
** \sa #Test_SendMsg_QueuePutError, #Test_SendMsg_PipeFull,
** \sa #Test_SendMsg_MsgLimitExceeded, #Test_SendMsg_GetPoolBufErr,
** \sa #Test_SendMsg_ZeroCopyGetPtr, #Test_SendMsg_ZeroCopySend,
** \sa #Test_SendMsg_ZeroCopyPass, #Test_SendMsg_ZeroCopyReleasePtr,
** \sa #Test_SendMsg_DisabledDestination, #Test_SendMsg_SendWithMetadata
**
******************************************************************************/
void Test_SendMsg_API(void);

/*****************************************************************************/
/**
** \brief Test response to sending a null message on the software bus
**
** \par Description
**        This function tests the response to sending a null message on the
**        software bus.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_SendMsg, #UT_GetNumEventsSent,
** \sa #UT_EventIsInHistory, #UT_Report
**
******************************************************************************/
void Test_SendMsg_NullPtr(void);

/*****************************************************************************/
/**
** \brief Test response to sending a message with an invalid ID
**
** \par Description
**        This function tests the response to sending a message with an invalid
**        ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_SendMsg,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #UT_Report
**
******************************************************************************/
void Test_SendMsg_InvalidMsgId(void);

/*****************************************************************************/
/**
** \brief Test response to sending a message which has no subscribers
**
** \par Description
**        This function tests the response to sending a message which has no
**        subscribers.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_SendMsg,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #UT_Report
**
******************************************************************************/
void Test_SendMsg_NoSubscribers(void);

/*****************************************************************************/
/**
** \brief Test response to sending a message with the message size larger
**        than allowed
**
** \par Description
**        This function tests the response to sending a message with the
**        message size larger than allowed.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_SendMsg,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #UT_Report
**
******************************************************************************/
void Test_SendMsg_MaxMsgSizePlusOne(void);

/*****************************************************************************/
/**
** \brief Test successfully sending a message on the software bus
**
** \par Description
**        This function tests successfully sending a message on the software
**        bus.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_Subscribe,
** \sa #CFE_SB_InitMsg, #CFE_SB_SendMsg, #UT_GetNumEventsSent,
** \sa #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_SendMsg_BasicSend(void);

/*****************************************************************************/
/**
** \brief Test successful send/receive for packet sequence count
**
** \par Description
**        This function tests successful send/receive for packet sequence
**        count.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_InitMsg,
** \sa #CFE_SB_Subscribe, #CCSDS_WR_SEQ, #CFE_SB_SendMsg, #CFE_SB_RcvMsg,
** \sa #CCSDS_RD_SEQ, #CFE_SB_PassMsg, #UT_GetNumEventsSent,
** \sa #UT_EventIsInHistory, #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_SendMsg_SequenceCount(void);

/*****************************************************************************/
/**
** \brief Test send message response to a socket queue 'put' error
**
** \par Description
**        This function tests the send message response to a socket queue 'put'
**        error.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_Subscribe,
** \sa #CFE_SB_InitMsg, #CFE_SB_SendMsg, #UT_GetNumEventsSent,
** \sa #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_SendMsg_QueuePutError(void);

/*****************************************************************************/
/**
** \brief Test send message response when the socket queue is full
**
** \par Description
**        This function tests the send message response when the socket queue
**        is full.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_CreatePipe,
** \sa #CFE_SB_Subscribe, #CFE_SB_SendMsg, #UT_GetNumEventsSent,
** \sa #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_SendMsg_PipeFull(void);

/*****************************************************************************/
/**
** \brief Test send message response to too many messages sent to the pipe
**
** \par Description
**        This function tests the send message response to too many messages
**        sent to the pipe.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_CreatePipe,
** \sa #CFE_SB_SubscribeEx, #CFE_SB_SendMsg, #UT_GetNumEventsSent,
** \sa #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_SendMsg_MsgLimitExceeded(void);

/*****************************************************************************/
/**
** \brief Test send message response to a buffer descriptor allocation failure
**
** \par Description
**        This function tests the send message response to a buffer descriptor
**        allocation failure.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_CreatePipe,
** \sa #CFE_SB_Subscribe, #UT_SetRtnCode, #CFE_SB_SendMsg,
** \sa #UT_GetNumEventsSent, #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_SendMsg_GetPoolBufErr(void);

/*****************************************************************************/
/**
** \brief Test getting a pointer to a buffer for zero copy mode with buffer
**        allocation failures
**
** \par Description
**        This function tests getting a pointer to a buffer for zero copy mode
**        with buffer allocation failures.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #UT_SetRtnCode, #CFE_SB_ZeroCopyGetPtr,
** \sa #UT_GetNumEventsSent, #UT_Report
**
******************************************************************************/
void Test_SendMsg_ZeroCopyGetPtr(void);

/*****************************************************************************/
/**
** \brief Test successfully sending a message in zero copy mode (telemetry
**        source sequence count increments)
**
** \par Description
**        This function tests successfully sending a message in zero copy mode
**        (telemetry source sequence count increments).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_Subscribe,
** \sa #CFE_SB_ZeroCopyGetPtr, #CFE_SB_InitMsg, #CCSDS_WR_SEQ,
** \sa #CFE_SB_ZeroCopySend, #CFE_SB_RcvMsg, #CCSDS_RD_SEQ,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #CFE_SB_DeletePipe,
** \sa #UT_Report
**
******************************************************************************/
void Test_SendMsg_ZeroCopySend(void);

/*****************************************************************************/
/**
** \brief Test successfully sending a message in zero copy mode (telemetry
**        source sequence count is unchanged)
**
** \par Description
**        This function tests successfully sending a message in zero copy mode
**        (telemetry source sequence count is unchanged).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_Subscribe,
** \sa #CFE_SB_ZeroCopyGetPtr, #CFE_SB_InitMsg, #CCSDS_WR_SEQ,
** \sa #CFE_SB_ZeroCopyPass, #CFE_SB_RcvMsg, #CCSDS_RD_SEQ,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #CFE_SB_DeletePipe,
** \sa #UT_Report
**
******************************************************************************/
void Test_SendMsg_ZeroCopyPass(void);

/*****************************************************************************/
/**
** \brief Test releasing a pointer to a buffer for zero copy mode
**
** \par Description
**        This function tests releasing a pointer to a buffer for zero copy
**        mode.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_ZeroCopyGetPtr,
** \sa #CFE_SB_ZeroCopyReleasePtr, #UT_SetRtnCode, #UT_GetNumEventsSent,
** \sa #UT_Report
**
******************************************************************************/
void Test_SendMsg_ZeroCopyReleasePtr(void);

/*****************************************************************************/
/**
** \brief Test send message response with the destination disabled
**
** \par Description
**        This function tests the send message response with the destination
**        disabled.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_Subscribe,
** \sa #CFE_SB_GetDestPtr, #CFE_SB_InitMsg, #CFE_SB_SendMsg,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #CFE_SB_DeletePipe,
** \sa #UT_Report
**
******************************************************************************/
void Test_SendMsg_DisabledDestination(void);

/*****************************************************************************/
/**
** \brief Test successfully sending a message with the metadata
**
** \par Description
**        This function tests successfully sending a message with the metadata.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_Subscribe,
** \sa #CFE_SB_InitMsg, #CFE_SB_SendMsgFull, #UT_GetNumEventsSent,
** \sa #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_SendMsg_SendWithMetadata(void);

/*****************************************************************************/
/**
** \brief Test response to sending a message with an invalid ID and ZeroCopy is
**        set
**
** \par Description
**        This function tests the response to sending a message with an invalid
**        ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_SendMsg,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #UT_Report
**
******************************************************************************/
void Test_SendMsg_InvalidMsgId_ZeroCopy(void);

/*****************************************************************************/
/**
** \brief Test response to sending a message which has no subscribers and
**        ZeroCopy is set
**
** \par Description
**        This function tests the response to sending a message which has no
**        subscribers.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_SendMsg,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #UT_Report
**
******************************************************************************/
void Test_SendMsg_NoSubscribers_ZeroCopy(void);

/*****************************************************************************/
/**
** \brief Test response to sending a message with the message size larger
**        than allowed and ZeroCopy is set
**
** \par Description
**        This function tests the response to sending a message with the
**        message size larger than allowed.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_SendMsg,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #UT_Report
**
******************************************************************************/
void Test_SendMsg_MaxMsgSizePlusOne_ZeroCopy(void);

/*****************************************************************************/
/**
** \brief Function for calling SB receive message API test functions
**
** \par Description
**        Function for calling SB receive message API test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #Test_RcvMsg_InvalidPipeId, #Test_RcvMsg_InvalidTimeout,
** \sa #Test_RcvMsg_Poll, #Test_RcvMsg_GetLastSenderNull,
** \sa #Test_RcvMsg_GetLastSenderInvalidPipe,
** \sa #Test_RcvMsg_GetLastSenderInvalidCaller,
** \sa #Test_RcvMsg_GetLastSenderSuccess, #Test_RcvMsg_Timeout,
** \sa #Test_RcvMsg_PipeReadError, #Test_RcvMsg_PendForever
**
******************************************************************************/
void Test_RcvMsg_API(void);

/*****************************************************************************/
/**
** \brief Test receiving a message from the software bus with an invalid
**        pipe ID
**
** \par Description
**        This function tests receiving a message from the software bus with an
**        invalid pipe ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_RcvMsg, #UT_GetNumEventsSent,
** \sa #UT_EventIsInHistory, #UT_Report
**
******************************************************************************/
void Test_RcvMsg_InvalidPipeId(void);

/*****************************************************************************/
/**
** \brief Test receiving a message response to an invalid timeout value (< -1)
**
** \par Description
**        This function tests receiving a message response to an invalid
**        timeout value (< -1).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_RcvMsg,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #CFE_SB_DeletePipe,
** \sa #UT_Report
**
******************************************************************************/
void Test_RcvMsg_InvalidTimeout(void);

/*****************************************************************************/
/**
** \brief Test receiving a message response when there is no message on
**        the queue
**
** \par Description
**        This function tests receiving a message response when there is no
**        message on the queue.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_RcvMsg,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #CFE_SB_DeletePipe,
** \sa #UT_Report
**
******************************************************************************/
void Test_RcvMsg_Poll(void);

/*****************************************************************************/
/**
** \brief Test receive last message response to a null sender ID
**
** \par Description
**        This function tests the receive last message response to a null
**        sender ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe,
** \sa #CFE_SB_GetLastSenderId, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_RcvMsg_GetLastSenderNull(void);

/*****************************************************************************/
/**
** \brief Test receive last message response to an invalid pipe ID
**
** \par Description
**        This function tests the receive last message response to an invalid
**        pipe ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe,
** \sa #CFE_SB_GetLastSenderId, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_RcvMsg_GetLastSenderInvalidPipe(void);

/*****************************************************************************/
/**
** \brief Test receive last message response to an invalid owner ID
**
** \par Description
**        This function tests the receive last message response to an invalid
**        owner ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe,
** \sa #CFE_SB_GetLastSenderId, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_RcvMsg_GetLastSenderInvalidCaller(void);

/*****************************************************************************/
/**
** \brief Test successful receive last message request
**
** \par Description
**        This function tests the successful receive last message request.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe,
** \sa #CFE_SB_GetLastSenderId, #UT_GetNumEventsSent, #CFE_SB_DeletePipe,
** \sa #UT_Report
**
******************************************************************************/
void Test_RcvMsg_GetLastSenderSuccess(void);

/*****************************************************************************/
/**
** \brief Test receiving a message response to a timeout
**
** \par Description
**        This function tests receiving a message response to a timeout.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #UT_SetRtnCode,
** \sa #CFE_SB_RcvMsg, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_RcvMsg_Timeout(void);

/*****************************************************************************/
/**
** \brief Test receiving a message response to a pipe read error
**
** \par Description
**        This function tests receiving a message response to a pipe read
**        error.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_RcvMsg,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #CFE_SB_DeletePipe,
** \sa #UT_Report
**
******************************************************************************/
void Test_RcvMsg_PipeReadError(void);

/*****************************************************************************/
/**
** \brief Test receiving a message response to a "pend forever" timeout
**
** \par Description
**        This function tests receiving a message response to a "pend forever"
**        timeout.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_InitMsg,
** \sa #CFE_SB_Subscribe, #CFE_SB_SendMsg, #CFE_SB_RcvMsg,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #CFE_SB_DeletePipe,
** \sa #UT_Report
**
******************************************************************************/
void Test_RcvMsg_PendForever(void);

/*****************************************************************************/
/**
** \brief Test receiving a message response to an invalid buffer pointer (null)
**
** \par Description
**        This function tests receiving a message response to an invalid buffer
**        pointer (null).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_RcvMsg,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #CFE_SB_DeletePipe,
** \sa #UT_Report
**
******************************************************************************/
void Test_RcvMsg_InvalidBufferPtr(void);

/*****************************************************************************/
/**
** \brief Test releasing zero copy buffers for all pipes owned by a
**        given app ID
**
** \par Description
**        This function tests releasing zero copy buffers for all pipes owned
**        by a given app ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_ZeroCopyGetPtr,
** \sa #CFE_SB_CleanUpApp, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #UT_Report
**
******************************************************************************/
void Test_CleanupApp_API(void);

/*****************************************************************************/
/**
** \brief Function for calling SB utility API test functions
**
** \par Description
**        Function for calling SB utility API test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #Test_CFE_SB_InitMsg, #Test_CFE_SB_MsgHdrSize,
** \sa #Test_CFE_SB_GetUserData, #Test_CFE_SB_SetGetMsgId,
** \sa #Test_CFE_SB_SetGetUserDataLength, #Test_CFE_SB_SetGetTotalMsgLength,
** \sa #Test_CFE_SB_SetGetMsgTime, #Test_CFE_SB_TimeStampMsg,
** \sa #Test_CFE_SB_SetGetCmdCode, #Test_CFE_SB_ChecksumUtils
**
******************************************************************************/
void Test_SB_Utils(void);

/*****************************************************************************/
/**
** \brief Function for calling SB init message test functions
**
** \par Description
**        Function for calling SB init message test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #Test_CFE_SB_InitMsg_True, #Test_CFE_SB_InitMsg_False
**
******************************************************************************/
void Test_CFE_SB_InitMsg(void);

/*****************************************************************************/
/**
** \brief Test message initialization, clearing the message content
**
** \par Description
**        This function tests message initialization (clearing the message
**        content).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #UT_Report
**
******************************************************************************/
void Test_CFE_SB_InitMsg_True(void);

/*****************************************************************************/
/**
** \brief Test message initialization, leaving the message content unchanged
**
** \par Description
**        This function tests message initialization (leaving the message
**        content unchanged).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #UT_Report
**
******************************************************************************/
void Test_CFE_SB_InitMsg_False(void);

/*****************************************************************************/
/**
** \brief Test getting the size of a command/telemetry message header
**
** \par Description
**        This function tests getting the size of a command/telemetry message
**        header.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_MsgHdrSize, #UT_Report
**
******************************************************************************/
void Test_CFE_SB_MsgHdrSize(void);

/*****************************************************************************/
/**
** \brief Test getting a pointer to the user data portion of a message
**
** \par Description
**        This function tests getting a pointer to the user data portion of a
**        message.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_SetMsgId, #CFE_SB_GetUserData,
** \sa #UT_Report
**
******************************************************************************/
void Test_CFE_SB_GetUserData(void);

/*****************************************************************************/
/**
** \brief Test setting and getting the message ID of a message
**
** \par Description
**        This function tests setting and getting the message ID of a message.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_SetMsgId, #CFE_SB_GetMsgId,
** \sa #UT_Report
**
******************************************************************************/
void Test_CFE_SB_SetGetMsgId(void);

/*****************************************************************************/
/**
** \brief Test setting and getting the user data size of a message
**
** \par Description
**        This function tests setting and getting the user data size of a
**        message.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_SetMsgId,
** \sa #CFE_SB_SetUserDataLength, #CFE_SB_GetUserDataLength,
** \sa #UT_GetActualPktLenField, #UT_Report
**
******************************************************************************/
void Test_CFE_SB_SetGetUserDataLength(void);

/*****************************************************************************/
/**
** \brief Test setting and getting the total message size
**
** \par Description
**        This function tests setting and getting the total message size.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_SetMsgId,
** \sa #CFE_SB_SetTotalMsgLength, #CFE_SB_GetTotalMsgLength,
** \sa #UT_GetActualPktLenField, #UT_Report
**
******************************************************************************/
void Test_CFE_SB_SetGetTotalMsgLength(void);

/*****************************************************************************/
/**
** \brief Test setting and getting the message time field
**
** \par Description
**        This function tests setting and getting the message time field.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_SetMsgId, #CFE_SB_SetMsgTime,
** \sa #CFE_SB_GetMsgTime, #UT_DisplayPkt, #UT_Report
**
******************************************************************************/
void Test_CFE_SB_SetGetMsgTime(void);

/*****************************************************************************/
/**
** \brief Test setting the time field to the current time
**
** \par Description
**        This function tests setting the time field to the current time.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_SetMsgId, #CFE_SB_TimeStampMsg,
** \sa #CFE_SB_GetMsgTime, #UT_DisplayPkt, #UT_Report
**
******************************************************************************/
void Test_CFE_SB_TimeStampMsg(void);

/*****************************************************************************/
/**
** \brief Test setting and getting the opcode field of message
**
** \par Description
**        This function tests setting and getting the opcode field of message.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_SetMsgId, #CFE_SB_SetCmdCode,
** \sa #CFE_SB_GetCmdCode, #UT_GetActualCmdCodeField, #UT_DisplayPkt,
** \sa #UT_Report
**
******************************************************************************/
void Test_CFE_SB_SetGetCmdCode(void);

/*****************************************************************************/
/**
** \brief Test generating, setting, getting, and validating a checksum field
**        for a message
**
** \par Description
**        This function tests generating, setting, getting, and validating a
**        checksum field for a message.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_GenerateChecksum,
** \sa #CFE_SB_GetChecksum, #CFE_SB_ValidateChecksum, #UT_DisplayPkt,
** \sa #CFE_SB_SetMsgId, #UT_Report
**
******************************************************************************/
void Test_CFE_SB_ChecksumUtils(void);

/*****************************************************************************/
/**
** \brief Function for calling SB special test cases functions
**
** \par Description
**        Function for calling SB special test cases functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #Test_OS_MutSem_ErrLogic, #Test_GetPipeName_ErrLogic,
** \sa #Test_ReqToSendEvent_ErrLogic, #Test_PutDestBlk_ErrLogic
**
******************************************************************************/
void Test_SB_SpecialCases(void);

/*****************************************************************************/
/**
** \brief Test pipe creation with semaphore take and give failures
**
** \par Description
**        This function tests pipe creation with semaphore take and give
**        failures.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #UT_SetRtnCode, #CFE_SB_CreatePipe,
** \sa #CFE_SB_Subscribe, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_OS_MutSem_ErrLogic(void);

/*****************************************************************************/
/**
** \brief Test getting a pipe name using an invalid pipe ID
**
** \par Description
**        This function tests getting a pipe name using an invalid pipe ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_GetPipeName,
** \sa #UT_GetNumEventsSent, #UT_EventIsInHistory, #CFE_SB_DeletePipe,
** \sa #UT_Report
**
******************************************************************************/
void Test_GetPipeName_ErrLogic(void);

/*****************************************************************************/
/**
** \brief Test successful recursive event prevention
**
** \par Description
**        This function tests successful recursive event prevention.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_RequestToSendEvent,
** \sa #UT_GetNumEventsSent, #UT_Report
**
******************************************************************************/
void Test_ReqToSendEvent_ErrLogic(void);

/*****************************************************************************/
/**
** \brief Test getting a destination descriptor to the SB memory pool using a
**        null destination pointer
**
** \par Description
**        This function tests getting a destination descriptor to the SB memory
**        pool using a null destination pointer.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_PutDestinationBlk,
** \sa #UT_GetNumEventsSent, #UT_Report
**
******************************************************************************/
void Test_PutDestBlk_ErrLogic(void);

/*****************************************************************************/
/**
** \brief Test internal function to get the pipe table index for the given pipe
**        ID
**
** \par Description
**        This function tests the internal function to get the pipe table index
**        for the given pipe ID when the ID is not in use.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_GetPipeIdx, #UT_GetNumEventsSent,
** \sa #UT_Report
**
******************************************************************************/
void Test_CFE_SB_GetPipeIdx(void);

/*****************************************************************************/
/**
** \brief Test functions that involve a buffer in the SB buffer pool
**
** \par Description
**        This function tests the functions that involve a buffer in the SB
**        buffer pool.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_GetBufferFromPool,
** \sa #CFE_SB_ReturnBufferToPool, #CFE_SB_DecrBufUseCnt,
** \sa #CFE_SB_PutDestinationBlk, #UT_GetNumEventsSent, #UT_Report
**
******************************************************************************/
void Test_CFE_SB_Buffers(void);

/*****************************************************************************/
/**
** \brief Test functions that involve bad pipe information
**
** \par Description
**        This function tests the functions that involve bad pipe information.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_DeletePipeFull,
** \sa #CFE_SB_SubscribeFull, #UT_GetNumEventsSent, #UT_Report
**
******************************************************************************/
void Test_CFE_SB_BadPipeInfo(void);

/*****************************************************************************/
/**
** \brief Test SendMsgFull function paths
**
** \par Description
**        This function tests branch paths in the SendMsgFull function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_InitMsg, #CFE_SB_ProcessCmdPipePkt,
** \sa #UT_EventIsInHistory, #CFE_SB_GetMsgId, #UT_SetRtnCode,
** \sa #UT_GetNumEventsSent, #CFE_SB_CreatePipe, #CFE_SB_Subscribe,
** \sa #CFE_SB_SendMsg, #CFE_SB_DeletePipe, #CFE_SB_SubscribeEx, #UT_Report
**
******************************************************************************/
void Test_SB_SendMsgPaths(void);

/*****************************************************************************/
/**
** \brief Test RcvMsg function unsubscribe/resubscribe path
**
** \par Description
**        This function tests the branch path in the RcvMsg function when a
**        message in the pipe is unsubscribed, then resubscribed.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #SB_ResetUnitTest, #CFE_SB_CreatePipe, #CFE_SB_InitMsg,
** \sa #CFE_SB_Subscribe, #CFE_SB_SendMsg, #CFE_SB_Unsubscribe,
** \sa #CFE_SB_RcvMsg, #UT_GetNumEventsSent, #UT_EventIsInHistory,
** \sa #CFE_SB_DeletePipe, #UT_Report
**
******************************************************************************/
void Test_RcvMsg_UnsubResubPath(void);

/*****************************************************************************/
/**
** \brief Test MessageStringSet and MessageStringGet function paths
**
** \par Description
**        This function tests the branch paths in the MessageStringSet and
**        MessageStringGet functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Report
**
******************************************************************************/
void Test_MessageString(void);
#endif /* _sb_ut_h_ */
