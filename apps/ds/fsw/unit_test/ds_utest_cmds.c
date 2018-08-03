/************************************************************************
**
** $Id: ds_utest_cmds.c 1.7.1.1 2015/02/28 17:13:36EST sstrege Exp  $
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
** Notes: 
**
**   Unit test for CFS Data Storage (DS) application source file "ds_cmds.c"
**
**   To direct text output to screen, 
**      enable '#define UTF_USE_STDOUT' in utf_custom.h
**
**   To direct text output to file, 
**      disable '#define UTF_USE_STDOUT' in utf_custom.h
** 
** $Log: ds_utest_cmds.c  $
** Revision 1.7.1.1 2015/02/28 17:13:36EST sstrege 
** Added copyright information
** Revision 1.7 2009/12/07 13:40:41EST lwalling 
** Update DS unit tests, add unit test results files to MKS
** Revision 1.6 2009/10/05 13:33:52EDT lwalling 
** Change basename string contents from required to optional
** Revision 1.5 2009/09/01 15:23:43EDT lwalling 
** Remove obsolete unit test, cleanup comments
** Revision 1.4 2009/08/27 16:32:26EDT lwalling 
** Updates from source code review
** Revision 1.3 2009/08/13 10:01:27EDT lwalling 
** Updates to unit test source files
** Revision 1.2 2009/08/07 16:25:02EDT lwalling 
** Update cmd tests, create table tests, modify makefile
** Revision 1.1 2009/05/26 13:37:48EDT lwalling 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/ds/fsw/unit_test/project.pj
** 
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/
#include "utf_custom.h"        /* UTF headers         */
#include "utf_types.h"
#include "utf_cfe_sb.h"
#include "utf_osapi.h"
#include "utf_osloader.h"
#include "utf_osfileapi.h"
#include "utf_cfe.h"

#include "ds_perfids.h"
#include "ds_msgids.h"
#include "ds_platform_cfg.h"

#include "ds_platform_cfg.h"
#include "ds_verify.h"

#include "ds_appdefs.h"

#include "ds_msg.h"
#include "ds_app.h"
#include "ds_cmds.h"
#include "ds_file.h"
#include "ds_table.h"
#include "ds_events.h"
#include "ds_version.h"

#include "cfe_es_cds.h"        /* cFE headers         */

#include <stdlib.h>            /* System headers      */

/************************************************************************
** Macro Definitions
*************************************************************************/
#define MESSAGE_FORMAT_IS_CCSDS

/************************************************************************
** Global data external to this file
*************************************************************************/

extern  DS_AppData_t DS_AppData;   /* DS app global data */

extern  uint32 UT_TotalTestCount;  /* Unit test global data */
extern  uint32 UT_TotalFailCount;

extern  DS_NoopCmd_t        *UT_NoopCmd;
extern  DS_ResetCmd_t       *UT_ResetCmd;
extern  DS_AppStateCmd_t    *UT_AppStateCmd;
extern  DS_FilterFileCmd_t  *UT_FilterFileCmd;
extern  DS_FilterTypeCmd_t  *UT_FilterTypeCmd;
extern  DS_FilterParmsCmd_t *UT_FilterParmsCmd;
extern  DS_DestTypeCmd_t    *UT_DestTypeCmd;
extern  DS_DestStateCmd_t   *UT_DestStateCmd;
extern  DS_DestPathCmd_t    *UT_DestPathCmd;
extern  DS_DestBaseCmd_t    *UT_DestBaseCmd;
extern  DS_DestExtCmd_t     *UT_DestExtCmd;
extern  DS_DestSizeCmd_t    *UT_DestSizeCmd;
extern  DS_DestAgeCmd_t     *UT_DestAgeCmd;
extern  DS_DestCountCmd_t   *UT_DestCountCmd;
extern  DS_CloseFileCmd_t   *UT_CloseFileCmd;

extern  DS_FilterTable_t     UT_FilterTbl;  
extern  DS_DestFileTable_t   UT_DestFileTbl;

extern  DS_FilterTable_t    *DS_FilterTblPtr;
extern  DS_DestFileTable_t  *DS_DestFileTblPtr;

/************************************************************************
** Local function prototypes
*************************************************************************/


/************************************************************************
** Local data
*************************************************************************/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Unit tests for source file ds_cmds.c                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void Test_cmds(void)
{
    uint16 CmdAcceptedCounter;
    uint16 CmdRejectedCounter;

    uint32 TestCount = 0;
    uint32 FailCount = 0;

    /*
    ** Tests for function DS_CmdNoop()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  good command packet
    */

    /* (1) invalid packet length (too short) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_NoopCmd, DS_CMD_MID, sizeof(DS_NoopCmd_t) - 1, FALSE);
    DS_CmdNoop((CFE_SB_MsgPtr_t) UT_NoopCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdNoop() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_NoopCmd, DS_CMD_MID, sizeof(DS_NoopCmd_t) + 1, FALSE);
    DS_CmdNoop((CFE_SB_MsgPtr_t) UT_NoopCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdNoop() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) good command packet, neither table is required */
    CmdAcceptedCounter = DS_AppData.CmdAcceptedCounter;
    CFE_SB_InitMsg(UT_NoopCmd, DS_CMD_MID, sizeof(DS_NoopCmd_t), FALSE);
    DS_CmdNoop((CFE_SB_MsgPtr_t) UT_NoopCmd);
    TestCount++;
    if (CmdAcceptedCounter == DS_AppData.CmdAcceptedCounter)
    {
        UTF_put_text("DS_CmdNoop() -- test failed (3)\n");
        FailCount++;
    }

    /*
    ** Tests for function DS_CmdReset()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  good command packet
    */

    /* (1) invalid packet length (too short) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_ResetCmd, DS_CMD_MID, sizeof(DS_ResetCmd_t) - 1, FALSE);
    DS_CmdReset((CFE_SB_MsgPtr_t) UT_ResetCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdReset() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_ResetCmd, DS_CMD_MID, sizeof(DS_ResetCmd_t) + 1, FALSE);
    DS_CmdReset((CFE_SB_MsgPtr_t) UT_ResetCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdReset() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) good command packet, neither table is required */
    CFE_SB_InitMsg(UT_ResetCmd, DS_CMD_MID, sizeof(DS_ResetCmd_t), FALSE);
    DS_CmdReset((CFE_SB_MsgPtr_t) UT_ResetCmd);
    TestCount++;
    if ((DS_AppData.CmdAcceptedCounter != 0) || (DS_AppData.CmdRejectedCounter != 0))
    {
        UTF_put_text("DS_CmdReset() -- test failed (3)\n");
        FailCount++;
    }

    /*
    ** Tests for function DS_CmdSetAppState()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  invalid enable/disable state (too large)
    **   (4)  good command packet
    */

    /* (1) invalid packet length (too short) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_AppStateCmd, DS_CMD_MID, sizeof(DS_AppStateCmd_t) - 1, TRUE);
    DS_CmdSetAppState((CFE_SB_MsgPtr_t) UT_AppStateCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetAppState() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_AppStateCmd, DS_CMD_MID, sizeof(DS_AppStateCmd_t) + 1, TRUE);
    DS_CmdSetAppState((CFE_SB_MsgPtr_t) UT_AppStateCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetAppState() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) invalid enable/disable state (too large) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_AppStateCmd, DS_CMD_MID, sizeof(DS_AppStateCmd_t), TRUE);
    UT_AppStateCmd->EnableState = DS_ENABLED + 1;
    DS_CmdSetAppState((CFE_SB_MsgPtr_t) UT_AppStateCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetAppState() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) good command packet, neither table is required */
    CmdAcceptedCounter = DS_AppData.CmdAcceptedCounter;
    CFE_SB_InitMsg(UT_AppStateCmd, DS_CMD_MID, sizeof(DS_AppStateCmd_t), TRUE);
    UT_AppStateCmd->EnableState = DS_ENABLED;
    DS_CmdSetAppState((CFE_SB_MsgPtr_t) UT_AppStateCmd);
    TestCount++;
    if (CmdAcceptedCounter == DS_AppData.CmdAcceptedCounter)
    {
        UTF_put_text("DS_CmdSetAppState() -- test failed (4)\n");
        FailCount++;
    }

    /*
    ** Tests for function DS_CmdSetFilterFile()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  invalid message ID (unused)
    **   (4)  invalid filter table parms index (too large)
    **   (5)  invalid destination file table index (too large)
    **   (6)  packet filter table not loaded
    **   (7)  invalid message ID (not in table)
    **   (8)  good command packet
    */

    /* (1) invalid packet length (too short) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_FilterFileCmd, DS_CMD_MID, sizeof(DS_FilterFileCmd_t) - 1, TRUE);
    DS_CmdSetFilterFile((CFE_SB_MsgPtr_t) UT_FilterFileCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetFilterFile() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_FilterFileCmd, DS_CMD_MID, sizeof(DS_FilterFileCmd_t) + 1, TRUE);
    DS_CmdSetFilterFile((CFE_SB_MsgPtr_t) UT_FilterFileCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetFilterFile() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) invalid packet filter message ID (unused) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_FilterFileCmd, DS_CMD_MID, sizeof(DS_FilterFileCmd_t), TRUE);
    UT_FilterFileCmd->MessageID = DS_UNUSED;
    UT_FilterFileCmd->FilterParmsIndex = DS_FILTERS_PER_PACKET - 1;
    UT_FilterFileCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    DS_CmdSetFilterFile((CFE_SB_MsgPtr_t) UT_FilterFileCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetFilterFile() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) invalid packet filter table parms index (too large) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_FilterFileCmd, DS_CMD_MID, sizeof(DS_FilterFileCmd_t), TRUE);
    UT_FilterFileCmd->MessageID = DS_HK_TLM_MID;
    UT_FilterFileCmd->FilterParmsIndex = DS_FILTERS_PER_PACKET;
    UT_FilterFileCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    DS_CmdSetFilterFile((CFE_SB_MsgPtr_t) UT_FilterFileCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetFilterFile() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) invalid destination file table index (too large) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_FilterFileCmd, DS_CMD_MID, sizeof(DS_FilterFileCmd_t), TRUE);
    UT_FilterFileCmd->MessageID = DS_HK_TLM_MID;
    UT_FilterFileCmd->FilterParmsIndex = DS_FILTERS_PER_PACKET - 1;
    UT_FilterFileCmd->FileTableIndex = DS_DEST_FILE_CNT;
    DS_CmdSetFilterFile((CFE_SB_MsgPtr_t) UT_FilterFileCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetFilterFile() -- test failed (5)\n");
        FailCount++;
    }

    /* (6) good command packet, but packet filter table not loaded */
    DS_AppData.FilterTblPtr = (DS_FilterTable_t *) NULL;
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_FilterFileCmd, DS_CMD_MID, sizeof(DS_FilterFileCmd_t), TRUE);
    UT_FilterFileCmd->MessageID = DS_HK_TLM_MID;
    UT_FilterFileCmd->FilterParmsIndex = DS_FILTERS_PER_PACKET - 1;
    UT_FilterFileCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    DS_CmdSetFilterFile((CFE_SB_MsgPtr_t) UT_FilterFileCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetFilterFile() -- test failed (6)\n");
        FailCount++;
    }
    DS_AppData.FilterTblPtr = DS_FilterTblPtr;

    /* (7) good command packet, packet filter table loaded, but entry not in use */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_FilterFileCmd, DS_CMD_MID, sizeof(DS_FilterFileCmd_t), TRUE);
    UT_FilterFileCmd->MessageID = DS_CMD_MID;
    UT_FilterFileCmd->FilterParmsIndex = DS_FILTERS_PER_PACKET - 1;
    UT_FilterFileCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    DS_CmdSetFilterFile((CFE_SB_MsgPtr_t) UT_FilterFileCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetFilterFile() -- test failed (7)\n");
        FailCount++;
    }

    /* (8) good command packet, packet filter table loaded, entry is in use */
    CmdAcceptedCounter = DS_AppData.CmdAcceptedCounter;
    CFE_SB_InitMsg(UT_FilterFileCmd, DS_CMD_MID, sizeof(DS_FilterFileCmd_t), TRUE);
    UT_FilterFileCmd->MessageID = DS_HK_TLM_MID;
    UT_FilterFileCmd->FilterParmsIndex = DS_FILTERS_PER_PACKET - 1;
    UT_FilterFileCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    DS_CmdSetFilterFile((CFE_SB_MsgPtr_t) UT_FilterFileCmd);
    TestCount++;
    if (CmdAcceptedCounter == DS_AppData.CmdAcceptedCounter)
    {
        UTF_put_text("DS_CmdSetFilterFile() -- test failed (8)\n");
        FailCount++;
    }

    /*
    ** Tests for function DS_CmdSetFilterType()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  invalid message ID (unused)
    **   (4)  invalid filter table parms index (too large)
    **   (5)  invalid filter type (too small)
    **   (6)  invalid filter type (too large)
    **   (7)  packet filter table not loaded
    **   (8)  invalid message ID (not in table)
    **   (9)  good command packet
    */

    /* (1) invalid packet length (too short) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_FilterTypeCmd, DS_CMD_MID, sizeof(DS_FilterTypeCmd_t) - 1, TRUE);
    DS_CmdSetFilterType((CFE_SB_MsgPtr_t) UT_FilterTypeCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetFilterType() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_FilterTypeCmd, DS_CMD_MID, sizeof(DS_FilterTypeCmd_t) + 1, TRUE);
    DS_CmdSetFilterType((CFE_SB_MsgPtr_t) UT_FilterTypeCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetFilterType() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) invalid packet filter message ID (unused) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_FilterTypeCmd, DS_CMD_MID, sizeof(DS_FilterTypeCmd_t), TRUE);
    UT_FilterTypeCmd->MessageID = DS_UNUSED;
    UT_FilterTypeCmd->FilterParmsIndex = DS_FILTERS_PER_PACKET - 1;
    UT_FilterTypeCmd->FilterType = DS_BY_TIME;
    DS_CmdSetFilterType((CFE_SB_MsgPtr_t) UT_FilterTypeCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetFilterType() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) invalid packet filter table parms index (too large) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_FilterTypeCmd, DS_CMD_MID, sizeof(DS_FilterTypeCmd_t), TRUE);
    UT_FilterTypeCmd->MessageID = DS_HK_TLM_MID;
    UT_FilterTypeCmd->FilterParmsIndex = DS_FILTERS_PER_PACKET;
    UT_FilterTypeCmd->FilterType = DS_BY_COUNT;
    DS_CmdSetFilterType((CFE_SB_MsgPtr_t) UT_FilterTypeCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetFilterType() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) invalid packet filter type (too small) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_FilterTypeCmd, DS_CMD_MID, sizeof(DS_FilterTypeCmd_t), TRUE);
    UT_FilterTypeCmd->MessageID = DS_HK_TLM_MID;
    UT_FilterTypeCmd->FilterParmsIndex = DS_FILTERS_PER_PACKET - 1;
    UT_FilterTypeCmd->FilterType = DS_BY_COUNT - 1;
    DS_CmdSetFilterType((CFE_SB_MsgPtr_t) UT_FilterTypeCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetFilterType() -- test failed (5)\n");
        FailCount++;
    }

    /* (6) invalid packet filter type (too large) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_FilterTypeCmd, DS_CMD_MID, sizeof(DS_FilterTypeCmd_t), TRUE);
    UT_FilterTypeCmd->MessageID = DS_HK_TLM_MID;
    UT_FilterTypeCmd->FilterParmsIndex = DS_FILTERS_PER_PACKET - 1;
    UT_FilterTypeCmd->FilterType = DS_BY_TIME + 1;
    DS_CmdSetFilterType((CFE_SB_MsgPtr_t) UT_FilterTypeCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetFilterType() -- test failed (6)\n");
        FailCount++;
    }

    /* (7) good command packet, but packet filter table not loaded */
    DS_AppData.FilterTblPtr = (DS_FilterTable_t *) NULL;
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_FilterTypeCmd, DS_CMD_MID, sizeof(DS_FilterTypeCmd_t), TRUE);
    UT_FilterTypeCmd->MessageID = DS_HK_TLM_MID;
    UT_FilterTypeCmd->FilterParmsIndex = DS_FILTERS_PER_PACKET - 1;
    UT_FilterTypeCmd->FilterType = DS_BY_TIME;
    DS_CmdSetFilterType((CFE_SB_MsgPtr_t) UT_FilterTypeCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetFilterType() -- test failed (7)\n");
        FailCount++;
    }
    DS_AppData.FilterTblPtr = DS_FilterTblPtr;

    /* (8) good command packet, packet filter table loaded, but entry not in use */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_FilterTypeCmd, DS_CMD_MID, sizeof(DS_FilterTypeCmd_t), TRUE);
    UT_FilterTypeCmd->MessageID = DS_CMD_MID;
    UT_FilterTypeCmd->FilterParmsIndex = DS_FILTERS_PER_PACKET - 1;
    UT_FilterTypeCmd->FilterType = DS_BY_TIME;
    DS_CmdSetFilterType((CFE_SB_MsgPtr_t) UT_FilterTypeCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetFilterType() -- test failed (8)\n");
        FailCount++;
    }

    /* (9) good command packet, packet filter table loaded, entry is in use */
    CmdAcceptedCounter = DS_AppData.CmdAcceptedCounter;
    CFE_SB_InitMsg(UT_FilterTypeCmd, DS_CMD_MID, sizeof(DS_FilterTypeCmd_t), TRUE);
    UT_FilterTypeCmd->MessageID = DS_HK_TLM_MID;
    UT_FilterTypeCmd->FilterParmsIndex = DS_FILTERS_PER_PACKET - 1;
    UT_FilterTypeCmd->FilterType = DS_BY_TIME;
    DS_CmdSetFilterType((CFE_SB_MsgPtr_t) UT_FilterTypeCmd);
    TestCount++;
    if (CmdAcceptedCounter == DS_AppData.CmdAcceptedCounter)
    {
        UTF_put_text("DS_CmdSetFilterType() -- test failed (9)\n");
        FailCount++;
    }

    /*
    ** Tests for function DS_CmdSetFilterParms()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  invalid message ID (unused)
    **   (4)  invalid filter table parms index (too large)
    **   (5)  invalid filter parameter value (N > X)
    **   (6)  packet filter table not loaded
    **   (7)  invalid message ID (not in table)
    **   (8)  good command packet
    */

    /* (1) invalid packet length (too short) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_FilterParmsCmd, DS_CMD_MID, sizeof(DS_FilterParmsCmd_t) - 1, TRUE);
    DS_CmdSetFilterParms((CFE_SB_MsgPtr_t) UT_FilterParmsCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetFilterParms() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_FilterParmsCmd, DS_CMD_MID, sizeof(DS_FilterParmsCmd_t) + 1, TRUE);
    DS_CmdSetFilterParms((CFE_SB_MsgPtr_t) UT_FilterParmsCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetFilterParms() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) invalid packet filter message ID (unused) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_FilterParmsCmd, DS_CMD_MID, sizeof(DS_FilterParmsCmd_t), TRUE);
    UT_FilterParmsCmd->MessageID = DS_UNUSED;
    UT_FilterParmsCmd->FilterParmsIndex = DS_FILTERS_PER_PACKET - 1;
    UT_FilterParmsCmd->Algorithm_N = 1;
    UT_FilterParmsCmd->Algorithm_X = 2;
    UT_FilterParmsCmd->Algorithm_O = 0;
    DS_CmdSetFilterParms((CFE_SB_MsgPtr_t) UT_FilterParmsCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetFilterParms() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) invalid packet filter table parms index (too large) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_FilterParmsCmd, DS_CMD_MID, sizeof(DS_FilterParmsCmd_t), TRUE);
    UT_FilterParmsCmd->MessageID = DS_HK_TLM_MID;
    UT_FilterParmsCmd->FilterParmsIndex = DS_FILTERS_PER_PACKET;
    UT_FilterParmsCmd->Algorithm_N = 1;
    UT_FilterParmsCmd->Algorithm_X = 2;
    UT_FilterParmsCmd->Algorithm_O = 1;
    DS_CmdSetFilterParms((CFE_SB_MsgPtr_t) UT_FilterParmsCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetFilterParms() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) invalid packet filter table parms algorithm N value */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_FilterParmsCmd, DS_CMD_MID, sizeof(DS_FilterParmsCmd_t), TRUE);
    UT_FilterParmsCmd->MessageID = DS_HK_TLM_MID;
    UT_FilterParmsCmd->FilterParmsIndex = DS_FILTERS_PER_PACKET - 1;
    UT_FilterParmsCmd->Algorithm_N = 2;
    UT_FilterParmsCmd->Algorithm_X = 1;
    UT_FilterParmsCmd->Algorithm_O = 0;
    DS_CmdSetFilterParms((CFE_SB_MsgPtr_t) UT_FilterParmsCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetFilterParms() -- test failed (5)\n");
        FailCount++;
    }

    /* (6) good command packet, but packet filter table not loaded */
    DS_AppData.FilterTblPtr = (DS_FilterTable_t *) NULL;
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_FilterParmsCmd, DS_CMD_MID, sizeof(DS_FilterParmsCmd_t), TRUE);
    UT_FilterParmsCmd->MessageID = DS_HK_TLM_MID;
    UT_FilterParmsCmd->FilterParmsIndex = DS_FILTERS_PER_PACKET - 1;
    UT_FilterParmsCmd->Algorithm_N = 1;
    UT_FilterParmsCmd->Algorithm_X = 2;
    UT_FilterParmsCmd->Algorithm_O = 0;
    DS_CmdSetFilterParms((CFE_SB_MsgPtr_t) UT_FilterParmsCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetFilterParms() -- test failed (6)\n");
        FailCount++;
    }
    DS_AppData.FilterTblPtr = DS_FilterTblPtr;

    /* (7) good command packet, packet filter table loaded, but entry not in use */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_FilterParmsCmd, DS_CMD_MID, sizeof(DS_FilterParmsCmd_t), TRUE);
    UT_FilterParmsCmd->MessageID = DS_CMD_MID;
    UT_FilterParmsCmd->FilterParmsIndex = DS_FILTERS_PER_PACKET - 1;
    UT_FilterParmsCmd->Algorithm_N = 1;
    UT_FilterParmsCmd->Algorithm_X = 2;
    UT_FilterParmsCmd->Algorithm_O = 0;
    DS_CmdSetFilterParms((CFE_SB_MsgPtr_t) UT_FilterParmsCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetFilterParms() -- test failed (7)\n");
        FailCount++;
    }

    /* (8) good command packet, packet filter table loaded, entry is in use */
    CmdAcceptedCounter = DS_AppData.CmdAcceptedCounter;
    CFE_SB_InitMsg(UT_FilterParmsCmd, DS_CMD_MID, sizeof(DS_FilterParmsCmd_t), TRUE);
    UT_FilterParmsCmd->MessageID = DS_HK_TLM_MID;
    UT_FilterParmsCmd->FilterParmsIndex = DS_FILTERS_PER_PACKET - 1;
    UT_FilterParmsCmd->Algorithm_N = 1;
    UT_FilterParmsCmd->Algorithm_X = 2;
    UT_FilterParmsCmd->Algorithm_O = 0;
    DS_CmdSetFilterParms((CFE_SB_MsgPtr_t) UT_FilterParmsCmd);
    TestCount++;
    if (CmdAcceptedCounter == DS_AppData.CmdAcceptedCounter)
    {
        UTF_put_text("DS_CmdSetFilterParms() -- test failed (8)\n");
        FailCount++;
    }

    /*
    ** Tests for function DS_CmdSetDestType()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  invalid destination file table index (too large)
    **   (4)  invalid filename type (too small)
    **   (5)  invalid filename type (too large)
    **   (6)  destination file table not loaded
    **   (7)  good command packet
    */

    /* (1) invalid packet length (too short) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestTypeCmd, DS_CMD_MID, sizeof(DS_DestTypeCmd_t) - 1, TRUE);
    DS_CmdSetDestType((CFE_SB_MsgPtr_t) UT_DestTypeCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestType() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestTypeCmd, DS_CMD_MID, sizeof(DS_DestTypeCmd_t) + 1, TRUE);
    DS_CmdSetDestType((CFE_SB_MsgPtr_t) UT_DestTypeCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestType() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) invalid destination file table index (too large) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestTypeCmd, DS_CMD_MID, sizeof(DS_DestTypeCmd_t), TRUE);
    UT_DestTypeCmd->FileTableIndex = DS_DEST_FILE_CNT;
    UT_DestTypeCmd->FileNameType = DS_BY_COUNT;
    DS_CmdSetDestType((CFE_SB_MsgPtr_t) UT_DestTypeCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestType() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) invalid destination filename type (too small) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestTypeCmd, DS_CMD_MID, sizeof(DS_DestTypeCmd_t), TRUE);
    UT_DestTypeCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    UT_DestTypeCmd->FileNameType = DS_BY_COUNT - 1;
    DS_CmdSetDestType((CFE_SB_MsgPtr_t) UT_DestTypeCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestType() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) invalid destination filename type (too large) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestTypeCmd, DS_CMD_MID, sizeof(DS_DestTypeCmd_t), TRUE);
    UT_DestTypeCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    UT_DestTypeCmd->FileNameType = DS_BY_TIME + 1;
    DS_CmdSetDestType((CFE_SB_MsgPtr_t) UT_DestTypeCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestType() -- test failed (5)\n");
        FailCount++;
    }

    /* (6) good command packet, but destination file table not loaded */
    DS_AppData.DestFileTblPtr = (DS_DestFileTable_t *) NULL;
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestTypeCmd, DS_CMD_MID, sizeof(DS_DestTypeCmd_t), TRUE);
    UT_DestTypeCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    UT_DestTypeCmd->FileNameType = DS_BY_TIME;
    DS_CmdSetDestType((CFE_SB_MsgPtr_t) UT_DestTypeCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestType() -- test failed (6)\n");
        FailCount++;
    }
    DS_AppData.DestFileTblPtr = DS_DestFileTblPtr;

    /* (7) good command packet, and destination file table is loaded */
    CmdAcceptedCounter = DS_AppData.CmdAcceptedCounter;
    CFE_SB_InitMsg(UT_DestTypeCmd, DS_CMD_MID, sizeof(DS_DestTypeCmd_t), TRUE);
    UT_DestTypeCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    UT_DestTypeCmd->FileNameType = DS_BY_TIME;
    DS_CmdSetDestType((CFE_SB_MsgPtr_t) UT_DestTypeCmd);
    TestCount++;
    if (CmdAcceptedCounter == DS_AppData.CmdAcceptedCounter)
    {
        UTF_put_text("DS_CmdSetDestType() -- test failed (7)\n");
        FailCount++;
    }

    /*
    ** Tests for function DS_CmdSetDestState()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  invalid destination file table index (too large)
    **   (4)  invalid enable/disable state (too large)
    **   (5)  destination file table not loaded
    **   (6)  good command packet
    */

    /* (1) invalid packet length (too short) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestStateCmd, DS_CMD_MID, sizeof(DS_DestStateCmd_t) - 1, TRUE);
    DS_CmdSetDestState((CFE_SB_MsgPtr_t) UT_DestStateCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestState() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestStateCmd, DS_CMD_MID, sizeof(DS_DestStateCmd_t) + 1, TRUE);
    DS_CmdSetDestState((CFE_SB_MsgPtr_t) UT_DestStateCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestState() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) invalid destination file table index (too large) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestStateCmd, DS_CMD_MID, sizeof(DS_DestStateCmd_t), TRUE);
    UT_DestStateCmd->FileTableIndex = DS_DEST_FILE_CNT;
    UT_DestStateCmd->EnableState = DS_ENABLED;
    DS_CmdSetDestState((CFE_SB_MsgPtr_t) UT_DestStateCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestState() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) invalid enable/disable state (too large) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestStateCmd, DS_CMD_MID, sizeof(DS_DestStateCmd_t), TRUE);
    UT_DestStateCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    UT_DestStateCmd->EnableState = DS_ENABLED + 1;
    DS_CmdSetDestState((CFE_SB_MsgPtr_t) UT_DestStateCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestState() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) good command packet, but destination file table not loaded */
    DS_AppData.DestFileTblPtr = (DS_DestFileTable_t *) NULL;
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestStateCmd, DS_CMD_MID, sizeof(DS_DestStateCmd_t), TRUE);
    UT_DestStateCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    UT_DestStateCmd->EnableState = DS_ENABLED;
    DS_CmdSetDestState((CFE_SB_MsgPtr_t) UT_DestStateCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestState() -- test failed (5)\n");
        FailCount++;
    }
    DS_AppData.DestFileTblPtr = DS_DestFileTblPtr;

    /* (6) good command packet, and destination file table is loaded */
    CmdAcceptedCounter = DS_AppData.CmdAcceptedCounter;
    CFE_SB_InitMsg(UT_DestStateCmd, DS_CMD_MID, sizeof(DS_DestStateCmd_t), TRUE);
    UT_DestStateCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    UT_DestStateCmd->EnableState = DS_ENABLED;
    DS_CmdSetDestState((CFE_SB_MsgPtr_t) UT_DestStateCmd);
    TestCount++;
    if (CmdAcceptedCounter == DS_AppData.CmdAcceptedCounter)
    {
        UTF_put_text("DS_CmdSetDestState() -- test failed (6)\n");
        FailCount++;
    }

    /*
    ** Tests for function DS_CmdSetDestPath()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  invalid destination file table index (too large)
    **   (4)  invalid destination pathname string (empty)
    **   (5)  invalid destination pathname string (no terminator)
    **   (6)  invalid destination pathname string (bad chars)
    **   (7)  destination file table not loaded
    **   (8)  good command packet
    */

    /* (1) invalid packet length (too short) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestPathCmd, DS_CMD_MID, sizeof(DS_DestPathCmd_t) - 1, TRUE);
    DS_CmdSetDestPath((CFE_SB_MsgPtr_t) UT_DestPathCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestPath() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestPathCmd, DS_CMD_MID, sizeof(DS_DestPathCmd_t) + 1, TRUE);
    DS_CmdSetDestPath((CFE_SB_MsgPtr_t) UT_DestPathCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestPath() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) invalid destination file table index (too large) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestPathCmd, DS_CMD_MID, sizeof(DS_DestPathCmd_t), TRUE);
    UT_DestPathCmd->FileTableIndex = DS_DEST_FILE_CNT;
    strcpy(UT_DestPathCmd->Pathname, "/this/is/valid/");
    DS_CmdSetDestPath((CFE_SB_MsgPtr_t) UT_DestPathCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestPath() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) invalid destination file pathname string (empty) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestPathCmd, DS_CMD_MID, sizeof(DS_DestPathCmd_t), TRUE);
    UT_DestPathCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    DS_CmdSetDestPath((CFE_SB_MsgPtr_t) UT_DestPathCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestPath() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) invalid destination file pathname string (no terminator) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestPathCmd, DS_CMD_MID, sizeof(DS_DestPathCmd_t), TRUE);
    UT_DestPathCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    strcpy(UT_DestPathCmd->Pathname, "/0123456789/0123456789/0123456789/0123456789/0123456789/0123456789/0123456789/");
    DS_CmdSetDestPath((CFE_SB_MsgPtr_t) UT_DestPathCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestPath() -- test failed (5)\n");
        FailCount++;
    }

    /* (6) invalid destination file pathname string (bad chars) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestPathCmd, DS_CMD_MID, sizeof(DS_DestPathCmd_t), TRUE);
    UT_DestPathCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    strcpy(UT_DestPathCmd->Pathname, "/this/is/not/valid/^&*()_+");
    DS_CmdSetDestPath((CFE_SB_MsgPtr_t) UT_DestPathCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestPath() -- test failed (6)\n");
        FailCount++;
    }

    /* (7) good command packet, but destination file table not loaded */
    DS_AppData.DestFileTblPtr = (DS_DestFileTable_t *) NULL;
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestPathCmd, DS_CMD_MID, sizeof(DS_DestPathCmd_t), TRUE);
    UT_DestPathCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    strcpy(UT_DestPathCmd->Pathname, "/this/is/valid/");
    DS_CmdSetDestPath((CFE_SB_MsgPtr_t) UT_DestPathCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestPath() -- test failed (7)\n");
        FailCount++;
    }
    DS_AppData.DestFileTblPtr = DS_DestFileTblPtr;

    /* (8) good command packet, and destination file table is loaded */
    CmdAcceptedCounter = DS_AppData.CmdAcceptedCounter;
    CFE_SB_InitMsg(UT_DestPathCmd, DS_CMD_MID, sizeof(DS_DestPathCmd_t), TRUE);
    UT_DestPathCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    strcpy(UT_DestPathCmd->Pathname, "/this/is/valid/");
    DS_CmdSetDestPath((CFE_SB_MsgPtr_t) UT_DestPathCmd);
    TestCount++;
    if (CmdAcceptedCounter == DS_AppData.CmdAcceptedCounter)
    {
        UTF_put_text("DS_CmdSetDestPath() -- test failed (8)\n");
        FailCount++;
    }

    /*
    ** Tests for function DS_CmdSetDestBase()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  invalid destination file table index (too large)
    **   (4)  invalid destination basename string (no terminator)
    **   (5)  invalid destination basename string (bad chars)
    **   (6)  destination file table not loaded
    **   (7)  good command packet
    */

    /* (1) invalid packet length (too short) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestBaseCmd, DS_CMD_MID, sizeof(DS_DestBaseCmd_t) - 1, TRUE);
    DS_CmdSetDestBase((CFE_SB_MsgPtr_t) UT_DestBaseCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestBase() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestBaseCmd, DS_CMD_MID, sizeof(DS_DestBaseCmd_t) + 1, TRUE);
    DS_CmdSetDestBase((CFE_SB_MsgPtr_t) UT_DestBaseCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestBase() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) invalid destination file table index (too large) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestBaseCmd, DS_CMD_MID, sizeof(DS_DestBaseCmd_t), TRUE);
    UT_DestBaseCmd->FileTableIndex = DS_DEST_FILE_CNT;
    strcpy(UT_DestBaseCmd->Basename, "this_is_valid");
    DS_CmdSetDestBase((CFE_SB_MsgPtr_t) UT_DestBaseCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestBase() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) invalid destination file basename string (no terminator) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestBaseCmd, DS_CMD_MID, sizeof(DS_DestBaseCmd_t), TRUE);
    UT_DestBaseCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    strcpy(UT_DestBaseCmd->Basename, "_0123456789_0123456789_0123456789_0123456789_0123456789_0123456789_0123456789_");
    DS_CmdSetDestBase((CFE_SB_MsgPtr_t) UT_DestBaseCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestBase() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) invalid destination file pathname string (bad chars) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestBaseCmd, DS_CMD_MID, sizeof(DS_DestBaseCmd_t), TRUE);
    UT_DestBaseCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    strcpy(UT_DestBaseCmd->Basename, "_this_is_not_valid_^&*()_+");
    DS_CmdSetDestBase((CFE_SB_MsgPtr_t) UT_DestBaseCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestBase() -- test failed (5)\n");
        FailCount++;
    }

    /* (6) good command packet, but destination file table not loaded */
    DS_AppData.DestFileTblPtr = (DS_DestFileTable_t *) NULL;
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestBaseCmd, DS_CMD_MID, sizeof(DS_DestBaseCmd_t), TRUE);
    UT_DestBaseCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    strcpy(UT_DestBaseCmd->Basename, "_this_is_valid_");
    DS_CmdSetDestBase((CFE_SB_MsgPtr_t) UT_DestBaseCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestBase() -- test failed (6)\n");
        FailCount++;
    }
    DS_AppData.DestFileTblPtr = DS_DestFileTblPtr;

    /* (7) good command packet, and destination file table is loaded */
    CmdAcceptedCounter = DS_AppData.CmdAcceptedCounter;
    CFE_SB_InitMsg(UT_DestBaseCmd, DS_CMD_MID, sizeof(DS_DestBaseCmd_t), TRUE);
    UT_DestBaseCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    strcpy(UT_DestBaseCmd->Basename, "_this_is_valid_");
    DS_CmdSetDestBase((CFE_SB_MsgPtr_t) UT_DestBaseCmd);
    TestCount++;
    if (CmdAcceptedCounter == DS_AppData.CmdAcceptedCounter)
    {
        UTF_put_text("DS_CmdSetDestBase() -- test failed (7)\n");
        FailCount++;
    }

    /*
    ** Tests for function DS_CmdSetDestExt()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  invalid destination file table index (too large)
    **   (4)  invalid destination filename extension string (no terminator)
    **   (5)  invalid destination filename extension string (bad chars)
    **   (6)  destination file table not loaded
    **   (7)  good command packet
    */

    /* (1) invalid command packet length (too short) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestExtCmd, DS_CMD_MID, sizeof(DS_DestExtCmd_t) - 1, TRUE);
    DS_CmdSetDestExt((CFE_SB_MsgPtr_t) UT_DestExtCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestExt() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid command packet length (too long) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestExtCmd, DS_CMD_MID, sizeof(DS_DestExtCmd_t) + 1, TRUE);
    DS_CmdSetDestExt((CFE_SB_MsgPtr_t) UT_DestExtCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestExt() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) invalid destination file table index (too large) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestExtCmd, DS_CMD_MID, sizeof(DS_DestExtCmd_t), TRUE);
    UT_DestExtCmd->FileTableIndex = DS_DEST_FILE_CNT;
    strcpy(UT_DestExtCmd->Extension, "OK");
    DS_CmdSetDestExt((CFE_SB_MsgPtr_t) UT_DestExtCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestExt() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) invalid destination filename extension string (no terminator) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestExtCmd, DS_CMD_MID, sizeof(DS_DestExtCmd_t), TRUE);
    UT_DestExtCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    strcpy(UT_DestExtCmd->Extension, "_0123456789_0123456789_");
    DS_CmdSetDestExt((CFE_SB_MsgPtr_t) UT_DestExtCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestExt() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) invalid destination filename extension string (bad chars) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestExtCmd, DS_CMD_MID, sizeof(DS_DestExtCmd_t), TRUE);
    UT_DestExtCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    strcpy(UT_DestExtCmd->Extension, "^&*()+");
    DS_CmdSetDestExt((CFE_SB_MsgPtr_t) UT_DestExtCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestExt() -- test failed (5)\n");
        FailCount++;
    }

    /* (6) good command packet, but destination file table not loaded */
    DS_AppData.DestFileTblPtr = (DS_DestFileTable_t *) NULL;
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestExtCmd, DS_CMD_MID, sizeof(DS_DestExtCmd_t), TRUE);
    UT_DestExtCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    strcpy(UT_DestExtCmd->Extension, "OK");
    DS_CmdSetDestExt((CFE_SB_MsgPtr_t) UT_DestExtCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestExt() -- test failed (6)\n");
        FailCount++;
    }
    DS_AppData.DestFileTblPtr = DS_DestFileTblPtr;

    /* (7) good command packet, and destination file table is loaded */
    CmdAcceptedCounter = DS_AppData.CmdAcceptedCounter;
    CFE_SB_InitMsg(UT_DestExtCmd, DS_CMD_MID, sizeof(DS_DestExtCmd_t), TRUE);
    UT_DestExtCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    strcpy(UT_DestExtCmd->Extension, "OK");
    DS_CmdSetDestExt((CFE_SB_MsgPtr_t) UT_DestExtCmd);
    TestCount++;
    if (CmdAcceptedCounter == DS_AppData.CmdAcceptedCounter)
    {
        UTF_put_text("DS_CmdSetDestExt() -- test failed (7)\n");
        FailCount++;
    }

    /*
    ** Tests for function DS_CmdSetDestSize()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  invalid destination file table index (too large)
    **   (4)  invalid destination max file size (too small)
    **   (5)  destination file table not loaded
    **   (6)  good command packet
    */

    /* (1) invalid packet length (too short) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestSizeCmd, DS_CMD_MID, sizeof(DS_DestSizeCmd_t) - 1, TRUE);
    DS_CmdSetDestSize((CFE_SB_MsgPtr_t) UT_DestSizeCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestSize() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestSizeCmd, DS_CMD_MID, sizeof(DS_DestSizeCmd_t) + 1, TRUE);
    DS_CmdSetDestSize((CFE_SB_MsgPtr_t) UT_DestSizeCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestSize() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) invalid destination file table index (too large) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestSizeCmd, DS_CMD_MID, sizeof(DS_DestSizeCmd_t), TRUE);
    UT_DestSizeCmd->FileTableIndex = DS_DEST_FILE_CNT;
    UT_DestSizeCmd->MaxFileSize = DS_FILE_MIN_SIZE_LIMIT;
    DS_CmdSetDestSize((CFE_SB_MsgPtr_t) UT_DestSizeCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestSize() -- test failed (3)\n");
        FailCount++;
    }

    /* (4)  invalid destination max file size (too small) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestSizeCmd, DS_CMD_MID, sizeof(DS_DestSizeCmd_t), TRUE);
    UT_DestSizeCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    UT_DestSizeCmd->MaxFileSize = DS_FILE_MIN_SIZE_LIMIT - 1;
    DS_CmdSetDestSize((CFE_SB_MsgPtr_t) UT_DestSizeCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestSize() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) good command packet, but destination file table not loaded */
    DS_AppData.DestFileTblPtr = (DS_DestFileTable_t *) NULL;
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestSizeCmd, DS_CMD_MID, sizeof(DS_DestSizeCmd_t), TRUE);
    UT_DestSizeCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    UT_DestSizeCmd->MaxFileSize = DS_FILE_MIN_SIZE_LIMIT;
    DS_CmdSetDestSize((CFE_SB_MsgPtr_t) UT_DestSizeCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestSize() -- test failed (5)\n");
        FailCount++;
    }
    DS_AppData.DestFileTblPtr = DS_DestFileTblPtr;

    /* (6) good command packet, and destination file table is loaded */
    CmdAcceptedCounter = DS_AppData.CmdAcceptedCounter;
    CFE_SB_InitMsg(UT_DestSizeCmd, DS_CMD_MID, sizeof(DS_DestSizeCmd_t), TRUE);
    UT_DestSizeCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    UT_DestSizeCmd->MaxFileSize = DS_FILE_MIN_SIZE_LIMIT;
    DS_CmdSetDestSize((CFE_SB_MsgPtr_t) UT_DestSizeCmd);
    TestCount++;
    if (CmdAcceptedCounter == DS_AppData.CmdAcceptedCounter)
    {
        UTF_put_text("DS_CmdSetDestSize() -- test failed (6)\n");
        FailCount++;
    }

    /*
    ** Tests for function DS_CmdSetDestAge()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  invalid destination file table index (too large)
    **   (4)  invalid destination max file age (too small)
    **   (5)  destination file table not loaded
    **   (6)  good command packet
    */

    /* (1) invalid packet length (too short) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestAgeCmd, DS_CMD_MID, sizeof(DS_DestAgeCmd_t) - 1, TRUE);
    DS_CmdSetDestAge((CFE_SB_MsgPtr_t) UT_DestAgeCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestAge() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestAgeCmd, DS_CMD_MID, sizeof(DS_DestAgeCmd_t) + 1, TRUE);
    DS_CmdSetDestAge((CFE_SB_MsgPtr_t) UT_DestAgeCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestAge() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) invalid destination file table index (too large) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestAgeCmd, DS_CMD_MID, sizeof(DS_DestAgeCmd_t), TRUE);
    UT_DestAgeCmd->FileTableIndex = DS_DEST_FILE_CNT;
    UT_DestAgeCmd->MaxFileAge = DS_FILE_MIN_AGE_LIMIT;
    DS_CmdSetDestAge((CFE_SB_MsgPtr_t) UT_DestAgeCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestAge() -- test failed (3)\n");
        FailCount++;
    }

    /* (4)  invalid destination max file age (too small) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestAgeCmd, DS_CMD_MID, sizeof(DS_DestAgeCmd_t), TRUE);
    UT_DestAgeCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    UT_DestAgeCmd->MaxFileAge = DS_FILE_MIN_AGE_LIMIT - 1;
    DS_CmdSetDestAge((CFE_SB_MsgPtr_t) UT_DestAgeCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestAge() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) good command packet, but destination file table not loaded */
    DS_AppData.DestFileTblPtr = (DS_DestFileTable_t *) NULL;
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestAgeCmd, DS_CMD_MID, sizeof(DS_DestAgeCmd_t), TRUE);
    UT_DestAgeCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    UT_DestAgeCmd->MaxFileAge = DS_FILE_MIN_AGE_LIMIT;
    DS_CmdSetDestAge((CFE_SB_MsgPtr_t) UT_DestAgeCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestAge() -- test failed (5)\n");
        FailCount++;
    }
    DS_AppData.DestFileTblPtr = DS_DestFileTblPtr;

    /* (6) good command packet, and destination file table is loaded */
    CmdAcceptedCounter = DS_AppData.CmdAcceptedCounter;
    CFE_SB_InitMsg(UT_DestAgeCmd, DS_CMD_MID, sizeof(DS_DestAgeCmd_t), TRUE);
    UT_DestAgeCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    UT_DestAgeCmd->MaxFileAge = DS_FILE_MIN_AGE_LIMIT;
    DS_CmdSetDestAge((CFE_SB_MsgPtr_t) UT_DestAgeCmd);
    TestCount++;
    if (CmdAcceptedCounter == DS_AppData.CmdAcceptedCounter)
    {
        UTF_put_text("DS_CmdSetDestAge() -- test failed (6)\n");
        FailCount++;
    }

    /*
    ** Tests for function DS_CmdSetDestCount()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  invalid destination file table index (too large)
    **   (4)  invalid destination sequence count (too large)
    **   (5)  destination file table not loaded
    **   (6)  good command packet
    */

    /* (1) invalid packet length (too short) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestCountCmd, DS_CMD_MID, sizeof(DS_DestCountCmd_t) - 1, TRUE);
    DS_CmdSetDestCount((CFE_SB_MsgPtr_t) UT_DestCountCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestCount() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestCountCmd, DS_CMD_MID, sizeof(DS_DestCountCmd_t) + 1, TRUE);
    DS_CmdSetDestCount((CFE_SB_MsgPtr_t) UT_DestCountCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestCount() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) invalid destination file table index (too large) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestCountCmd, DS_CMD_MID, sizeof(DS_DestCountCmd_t), TRUE);
    UT_DestCountCmd->FileTableIndex = DS_DEST_FILE_CNT;
    UT_DestCountCmd->SequenceCount = DS_MAX_SEQUENCE_COUNT;
    DS_CmdSetDestCount((CFE_SB_MsgPtr_t) UT_DestCountCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestCount() -- test failed (3)\n");
        FailCount++;
    }

    /* (4)  invalid destination sequence count (too large) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestCountCmd, DS_CMD_MID, sizeof(DS_DestCountCmd_t), TRUE);
    UT_DestCountCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    UT_DestCountCmd->SequenceCount = DS_MAX_SEQUENCE_COUNT + 1;
    DS_CmdSetDestCount((CFE_SB_MsgPtr_t) UT_DestCountCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestCount() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) good command packet, but destination file table not loaded */
    DS_AppData.DestFileTblPtr = (DS_DestFileTable_t *) NULL;
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_DestCountCmd, DS_CMD_MID, sizeof(DS_DestCountCmd_t), TRUE);
    UT_DestCountCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    UT_DestCountCmd->SequenceCount = DS_MAX_SEQUENCE_COUNT;
    DS_CmdSetDestCount((CFE_SB_MsgPtr_t) UT_DestCountCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdSetDestCount() -- test failed (5)\n");
        FailCount++;
    }
    DS_AppData.DestFileTblPtr = DS_DestFileTblPtr;

    /* (6) good command packet, and destination file table is loaded */
    CmdAcceptedCounter = DS_AppData.CmdAcceptedCounter;
    CFE_SB_InitMsg(UT_DestCountCmd, DS_CMD_MID, sizeof(DS_DestCountCmd_t), TRUE);
    UT_DestCountCmd->FileTableIndex = DS_DEST_FILE_CNT - 1;
    UT_DestCountCmd->SequenceCount = DS_MAX_SEQUENCE_COUNT;
    DS_CmdSetDestCount((CFE_SB_MsgPtr_t) UT_DestCountCmd);
    TestCount++;
    if (CmdAcceptedCounter == DS_AppData.CmdAcceptedCounter)
    {
        UTF_put_text("DS_CmdSetDestCount() -- test failed (6)\n");
        FailCount++;
    }

    /*
    ** Tests for function DS_CmdCloseFile()...
    **
    **   (1)  invalid command packet length (too short)
    **   (2)  invalid command packet length (too long)
    **   (3)  invalid destination file table index (too large)
    **   (4)  good command packet
    */

    /* (1) invalid packet length (too short) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_CloseFileCmd, DS_CMD_MID, sizeof(DS_CloseFileCmd_t) - 1, TRUE);
    DS_CmdCloseFile((CFE_SB_MsgPtr_t) UT_CloseFileCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdCloseFile() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid packet length (too long) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_CloseFileCmd, DS_CMD_MID, sizeof(DS_CloseFileCmd_t) + 1, TRUE);
    DS_CmdCloseFile((CFE_SB_MsgPtr_t) UT_CloseFileCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdCloseFile() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) invalid destination file table index (too large) */
    CmdRejectedCounter = DS_AppData.CmdRejectedCounter;
    CFE_SB_InitMsg(UT_CloseFileCmd, DS_CMD_MID, sizeof(DS_CloseFileCmd_t), TRUE);
    UT_CloseFileCmd->FileTableIndex = DS_DEST_FILE_CNT;
    DS_CmdCloseFile((CFE_SB_MsgPtr_t) UT_CloseFileCmd);
    TestCount++;
    if (CmdRejectedCounter == DS_AppData.CmdRejectedCounter)
    {
        UTF_put_text("DS_CmdCloseFile() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) good command packet, destination file table not required */
    CmdAcceptedCounter = DS_AppData.CmdAcceptedCounter;
    CFE_SB_InitMsg(UT_CloseFileCmd, DS_CMD_MID, sizeof(DS_CloseFileCmd_t), TRUE);
    UT_CloseFileCmd->FileTableIndex = 1;
    DS_CmdCloseFile((CFE_SB_MsgPtr_t) UT_CloseFileCmd);
    TestCount++;
    if (CmdAcceptedCounter == DS_AppData.CmdAcceptedCounter)
    {
        UTF_put_text("DS_CmdCloseFile() -- test failed (4)\n");
        FailCount++;
    }


    /*
    ** Summary for this group of unit tests...
    */
    UTF_put_text("ds_cmds.c -- test count = %d, test errors = %d\n", TestCount, FailCount);

    UT_TotalTestCount += TestCount;
    UT_TotalFailCount += FailCount;

    return;

} /* End of Test_cmds() */








