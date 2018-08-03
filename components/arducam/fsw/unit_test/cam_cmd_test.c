/*
  Copyright (C) 2009 - 2016 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

  This software is provided "as is" without any warranty of any, kind either express, implied, or statutory, including, but not
  limited to, any warranty that the software will conform to, specifications any implied warranties of merchantability, fitness
  for a particular purpose, and freedom from infringement, and any warranty that the documentation will conform to the program, or
  any warranty that the software will be error free.

  In no event shall NASA be liable for any damages, including, but not limited to direct, indirect, special or consequential damages,
  arising out of, resulting from, or in any way connected with the software or its documentation.  Whether or not based upon warranty,
  contract, tort or otherwise, and whether or not loss was sustained from, or arose out of the results of, or use of, the software,
  documentation or services provided hereunder

  ITC Team
  NASA IV&V
  ivv-itc@lists.nasa.gov
*/

#include "cam_cmd_test.h"
#include "cam_test_utils.h"

#include <cam_app.h>
#include <cam_msgids.h>
#include <cam_platform_cfg.h>

#include <uttest.h>
#include <utassert.h>
#include <ut_cfe_sb_hooks.h>
#include <ut_cfe_sb_stubs.h>

#include <stdio.h>

/* test noop cmd */
static void CAM_Cmd_Test_NOOP(void)
{
    /* init data */
    CAM_AppData.HkTelemetryPkt.CommandCount = 10;
    CAM_AppData.HkTelemetryPkt.CommandErrorCount = 20;

    /* init noop cmd */
    CAM_NoArgsCmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, CAM_CMD_MID, sizeof(CAM_NoArgsCmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, CAM_NOOP_CC);

    /* process cmd */
    CAM_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&cmd;
    CAM_ProcessCommandPacket();
    
    /* cmd counters */
    UtAssert_True(CAM_AppData.HkTelemetryPkt.CommandCount == 11, "cam cmd count");
    UtAssert_True(CAM_AppData.HkTelemetryPkt.CommandErrorCount == 20, "cam cmd error count");
}

/* test reset counters cmd */
static void CAM_Cmd_Test_RESET_COUNTERS(void)
{
    /* init data */
    CAM_AppData.HkTelemetryPkt.CommandCount = 10;
    CAM_AppData.HkTelemetryPkt.CommandErrorCount = 20;

    /* init reset counters cmd */
    CAM_NoArgsCmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, CAM_CMD_MID, sizeof(CAM_NoArgsCmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, CAM_RESET_COUNTERS_CC);

    /* process cmd */
    CAM_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&cmd;
    CAM_ProcessCommandPacket();
    
    /* cmd counters */
    UtAssert_True(CAM_AppData.HkTelemetryPkt.CommandCount == 0, "cam cmd count");
    UtAssert_True(CAM_AppData.HkTelemetryPkt.CommandErrorCount == 0, "cam cmd error count");
}

/* test stop cmd */
static void CAM_Cmd_Test_STOP(void)
{
    /* init data */
    CAM_AppData.HkTelemetryPkt.CommandCount = 10;
    CAM_AppData.HkTelemetryPkt.CommandErrorCount = 20;

    /* init stop cmd */
    CAM_NoArgsCmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, CAM_CMD_MID, sizeof(CAM_NoArgsCmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, CAM_STOP_CC);

    /* process cmd */
    CAM_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&cmd;
    CAM_ProcessCommandPacket();

    /* cmd counters */
    UtAssert_True(CAM_AppData.HkTelemetryPkt.CommandCount == 11, "cam cmd count");
    UtAssert_True(CAM_AppData.HkTelemetryPkt.CommandErrorCount == 20, "cam cmd error count");

    /* app data */
    UtAssert_True(CAM_AppData.State == CAM_STOP, "cam stopped");
}

/* test pause cmd */
static void CAM_Cmd_Test_PAUSE(void)
{
    /* init data */
    CAM_AppData.HkTelemetryPkt.CommandCount = 10;
    CAM_AppData.HkTelemetryPkt.CommandErrorCount = 20;
    CAM_AppData.State = CAM_STOP;

    /* init pause cmd */
    CAM_NoArgsCmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, CAM_CMD_MID, sizeof(CAM_NoArgsCmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, CAM_PAUSE_CC);

    /* process cmd */
    CAM_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&cmd;
    CAM_ProcessCommandPacket();
    
    /* cmd counters */
    UtAssert_True(CAM_AppData.HkTelemetryPkt.CommandCount == 11, "cam cmd count");
    UtAssert_True(CAM_AppData.HkTelemetryPkt.CommandErrorCount == 20, "cam cmd error count");

    /* app data */
    UtAssert_True(CAM_AppData.State == CAM_PAUSE, "cam paused");
}

/* test resume cmd */
static void CAM_Cmd_Test_RESUME(void)
{
    /* init data */
    CAM_AppData.HkTelemetryPkt.CommandCount = 10;
    CAM_AppData.HkTelemetryPkt.CommandErrorCount = 20;
    CAM_AppData.State = CAM_STOP;

    /* init resume cmd */
    CAM_NoArgsCmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, CAM_CMD_MID, sizeof(CAM_NoArgsCmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, CAM_RESUME_CC);

    /* process cmd */
    CAM_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&cmd;
    CAM_ProcessCommandPacket();
    
    /* cmd counters */
    UtAssert_True(CAM_AppData.HkTelemetryPkt.CommandCount == 11, "cam cmd count");
    UtAssert_True(CAM_AppData.HkTelemetryPkt.CommandErrorCount == 20, "cam cmd error count");

    /* app data */
    UtAssert_True(CAM_AppData.State == CAM_RUN, "cam running");
}

/* test timeout cmd */
static void CAM_Cmd_Test_TIMEOUT(void)
{
	/* init data */
	CAM_AppData.HkTelemetryPkt.CommandCount = 10;
    CAM_AppData.HkTelemetryPkt.CommandErrorCount = 20;
    CAM_AppData.State = CAM_RUN;

	/* init timeout cmd */
	CAM_NoArgsCmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, CAM_CMD_MID, sizeof(CAM_NoArgsCmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, CAM_TIMEOUT_CC);

    /* process cmd */
    CAM_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&cmd;
    CAM_ProcessCommandPacket();

	 /* cmd counters */
    UtAssert_True(CAM_AppData.HkTelemetryPkt.CommandCount == 11, "cam cmd count");
    UtAssert_True(CAM_AppData.HkTelemetryPkt.CommandErrorCount == 20, "cam cmd error count");

    /* app data */
    UtAssert_True(CAM_AppData.State == CAM_TIME, "cam timed out");
}

/* test low voltage cmd */
static void CAM_Cmd_Test_LOW_VOLTAGE(void)
{
	/* init data */
	CAM_AppData.HkTelemetryPkt.CommandCount = 10;
    CAM_AppData.HkTelemetryPkt.CommandErrorCount = 20;
    CAM_AppData.State = CAM_RUN;

	/* init timeout cmd */
	CAM_NoArgsCmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, CAM_CMD_MID, sizeof(CAM_NoArgsCmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, CAM_LOW_VOLTAGE_CC);

    /* process cmd */
    CAM_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&cmd;
    CAM_ProcessCommandPacket();

	 /* cmd counters */
    UtAssert_True(CAM_AppData.HkTelemetryPkt.CommandCount == 11, "cam cmd count");
    UtAssert_True(CAM_AppData.HkTelemetryPkt.CommandErrorCount == 20, "cam cmd error count");

    /* app data */
    UtAssert_True(CAM_AppData.State == CAM_LOW_VOLTAGE, "cam low voltage");
}

/* test exp 1 cmd */
static void CAM_Cmd_Test_EXP1(void)
{
	/* init data */
	CAM_AppData.HkTelemetryPkt.CommandCount = 10;
    CAM_AppData.HkTelemetryPkt.CommandErrorCount = 20;
    CAM_AppData.State = CAM_STOP;

	/* init timeout cmd */
	CAM_NoArgsCmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, CAM_CMD_MID, sizeof(CAM_NoArgsCmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, CAM_EXP1_CC);

    /* process cmd */
    CAM_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&cmd;
    CAM_ProcessCommandPacket();

	 /* cmd counters */
    UtAssert_True(CAM_AppData.HkTelemetryPkt.CommandCount == 11, "cam cmd count");
    UtAssert_True(CAM_AppData.HkTelemetryPkt.CommandErrorCount == 20, "cam cmd error count");

    /* app data */
    UtAssert_True(CAM_AppData.Exp == 1, "cam exp1");
	UtAssert_True(CAM_AppData.State == CAM_STOP, "cam stop");
}

/* test exp 2 cmd */
static void CAM_Cmd_Test_EXP2(void)
{
	/* init data */
	CAM_AppData.HkTelemetryPkt.CommandCount = 10;
    CAM_AppData.HkTelemetryPkt.CommandErrorCount = 20;
    CAM_AppData.State = CAM_PAUSE;

	/* init timeout cmd */
	CAM_NoArgsCmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, CAM_CMD_MID, sizeof(CAM_NoArgsCmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, CAM_EXP2_CC);

    /* process cmd */
    CAM_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&cmd;
    CAM_ProcessCommandPacket();

	 /* cmd counters */
    UtAssert_True(CAM_AppData.HkTelemetryPkt.CommandCount == 11, "cam cmd count");
    UtAssert_True(CAM_AppData.HkTelemetryPkt.CommandErrorCount == 20, "cam cmd error count");

    /* app data */
    UtAssert_True(CAM_AppData.Exp == 2, "cam exp 2");
	UtAssert_True(CAM_AppData.State == CAM_PAUSE, "cam pause");
}

/* test exp 3 cmd */
static void CAM_Cmd_Test_EXP3(void)
{
	/* init data */
	CAM_AppData.HkTelemetryPkt.CommandCount = 10;
    CAM_AppData.HkTelemetryPkt.CommandErrorCount = 20;
    CAM_AppData.State = CAM_TIME;

	/* init timeout cmd */
	CAM_NoArgsCmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, CAM_CMD_MID, sizeof(CAM_NoArgsCmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, CAM_EXP3_CC);

    /* process cmd */
    CAM_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&cmd;
    CAM_ProcessCommandPacket();

	 /* cmd counters */
    UtAssert_True(CAM_AppData.HkTelemetryPkt.CommandCount == 11, "cam cmd count");
    UtAssert_True(CAM_AppData.HkTelemetryPkt.CommandErrorCount == 20, "cam cmd error count");

    /* app data */
    UtAssert_True(CAM_AppData.Exp == 3, "cam exp 3");
	UtAssert_True(CAM_AppData.State == CAM_TIME, "cam time");
}

/* test send HkTelemetryPkt cmd */
static void CAM_Cmd_Test_HK(void)
{
    /* init data */
    Ut_CFE_SB_InitMsgHook(&CAM_AppData.HkTelemetryPkt, CAM_HK_TLM_MID, CAM_HK_TLM_LNGTH, TRUE);
    CAM_AppData.HkTelemetryPkt.CommandCount = 10;
    CAM_AppData.HkTelemetryPkt.CommandErrorCount = 20;

    /* init HkTelemetryPkt cmd */
    CAM_NoArgsCmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, CAM_SEND_HK_MID, sizeof(CAM_NoArgsCmd_t), TRUE);

    /* process cmd */
    CAM_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&cmd;
    CAM_ProcessCommandPacket();
    
    /* cmd counters */
    UtAssert_True(CAM_AppData.HkTelemetryPkt.CommandCount == 10, "cam cmd count");
    UtAssert_True(CAM_AppData.HkTelemetryPkt.CommandErrorCount == 20, "cam cmd error count");

    /* app data */
    UtAssert_PacketSent(CAM_HK_TLM_MID, "cam HkTelemetryPkt sent");
    CAM_Hk_tlm_t *HkTelemetryPkt = (CAM_Hk_tlm_t*)Ut_CFE_SB_FindPacket(CAM_HK_TLM_MID, 1);
    UtAssert_True(HkTelemetryPkt != NULL, "cam HkTelemetryPkt packet");
    if(HkTelemetryPkt)
    {
        UtAssert_True(HkTelemetryPkt->CommandCount == 10, "cam HkTelemetryPkt cmd error count");
        UtAssert_True(HkTelemetryPkt->CommandErrorCount == 20, "cam HkTelemetryPkt cmd error count");
    }
}

/* test invalid cmd code */
static void CAM_Cmd_Test_INVALID_CC(void)
{
    /* init data */
    CAM_AppData.HkTelemetryPkt.CommandCount = 10;
    CAM_AppData.HkTelemetryPkt.CommandErrorCount = 20;

    /* init invalid cmd */
    CAM_NoArgsCmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, CAM_CMD_MID, sizeof(CAM_NoArgsCmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, 100);

    /* process cmd */
    CAM_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&cmd;
    CAM_ProcessCommandPacket();
    
    /* cmd counters */
    UtAssert_True(CAM_AppData.HkTelemetryPkt.CommandCount == 10, "cam cmd count");
    UtAssert_True(CAM_AppData.HkTelemetryPkt.CommandErrorCount == 21, "cam cmd error count");

    /* app data */
}

/* test invalid msg id */
static void CAM_Cmd_Test_INVALID_MSG(void)
{
    /* init data */
    CAM_AppData.HkTelemetryPkt.CommandCount = 10;
    CAM_AppData.HkTelemetryPkt.CommandErrorCount = 20;

    /* init invalid cmd */
    CAM_NoArgsCmd_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, 50, sizeof(CAM_NoArgsCmd_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, CAM_NOOP_CC);

    /* process cmd */
    CAM_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&cmd;
    CAM_ProcessCommandPacket();
    
    /* cmd counters */
    UtAssert_True(CAM_AppData.HkTelemetryPkt.CommandCount == 10, "cam cmd count");
    UtAssert_True(CAM_AppData.HkTelemetryPkt.CommandErrorCount == 21, "cam cmd error count");

    /* app data */
}

/* test invalid length */
static void CAM_Cmd_Test_INVALID_LENGTH(void)
{
    /* init data */
    CAM_AppData.HkTelemetryPkt.CommandCount = 10;
    CAM_AppData.HkTelemetryPkt.CommandErrorCount = 20;

    /* init invalid cmd */
    CAM_Exp_tlm_t cmd;
    Ut_CFE_SB_InitMsgHook(&cmd, CAM_CMD_MID, sizeof(CAM_Exp_tlm_t), TRUE);
    Ut_CFE_SB_SetCmdCodeHook((CFE_SB_MsgPtr_t)&cmd, CAM_NOOP_CC);

    /* process cmd */
    CAM_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&cmd;
    CAM_ProcessCommandPacket();
    
    /* cmd counters */
    UtAssert_True(CAM_AppData.HkTelemetryPkt.CommandCount == 10, "cam cmd count");
    UtAssert_True(CAM_AppData.HkTelemetryPkt.CommandErrorCount == 21, "cam cmd error count");
}

void CAM_Cmd_Test_AddTestCases(void)
{
    UtTest_Add(CAM_Cmd_Test_NOOP, CAM_Test_Setup, CAM_Test_TearDown,
        "Cam Ground Command: NOOP");

    UtTest_Add(CAM_Cmd_Test_RESET_COUNTERS, CAM_Test_Setup, CAM_Test_TearDown,
        "Cam Ground Command: RESET COUNTERS");

    UtTest_Add(CAM_Cmd_Test_STOP, CAM_Test_Setup, CAM_Test_TearDown,
        "Cam Ground Command: STOP");

    UtTest_Add(CAM_Cmd_Test_PAUSE, CAM_Test_Setup, CAM_Test_TearDown,
        "Cam Ground Command: PAUSE");

    UtTest_Add(CAM_Cmd_Test_RESUME, CAM_Test_Setup, CAM_Test_TearDown,
        "Cam Ground Command: RESUME");

    UtTest_Add(CAM_Cmd_Test_HK, CAM_Test_Setup, CAM_Test_TearDown,
        "Cam Ground Command: HKTELEMETRY PKT");

	UtTest_Add(CAM_Cmd_Test_TIMEOUT, CAM_Test_Setup, CAM_Test_TearDown,
		"Cam Ground Command: TIMEOUT");

	UtTest_Add(CAM_Cmd_Test_LOW_VOLTAGE, CAM_Test_Setup, CAM_Test_TearDown,
		"Cam Ground Command: LOW VOLTAGE");

	UtTest_Add(CAM_Cmd_Test_EXP1, CAM_Test_Setup, CAM_Test_TearDown,
		"Cam Ground Command: EXP 1");

	UtTest_Add(CAM_Cmd_Test_EXP2, CAM_Test_Setup, CAM_Test_TearDown,
		"Cam Ground Command: EXP 2");

	UtTest_Add(CAM_Cmd_Test_EXP3, CAM_Test_Setup, CAM_Test_TearDown,
		"Cam Ground Command: EXP 3");

	UtTest_Add(CAM_Cmd_Test_INVALID_CC, CAM_Test_Setup, CAM_Test_TearDown,
        "Cam Ground Command: INVALID CMD CODE");

    UtTest_Add(CAM_Cmd_Test_INVALID_MSG, CAM_Test_Setup, CAM_Test_TearDown,
        "Cam Ground Command: INVALID MSG");

    UtTest_Add(CAM_Cmd_Test_INVALID_LENGTH, CAM_Test_Setup, CAM_Test_TearDown,
        "Cam Ground Command: INVALID LENGTH");
}

