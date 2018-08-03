#========================================================================================
# File:    app_code_src.py
# Author:  Tam Ngo/JSC
# Date:    2012-02-22
#
# Modification History:
#   Date | Author | Description
#   ---------------------------
#   10/7/15 | Susanne Strege | Added msg.h header definition and generation.  Updated 
#                              app.h and app.c definitions.
#
#   03/31/16 | Michael Rosburg | Updated app.h and app.c definitions. Changed SCH pipe
#                                definition.
#   05/25/16 | Michael Rosburg | Removed pipe depth macros definition, put in _platform_cfg.h
#   05/26/16 | Michael Rosburg | Add CFE_ES_WaitForStartupSync to _AppMain 1000msec and
#                                Enhance Performance Monitoring for the application
#========================================================================================

import os, app_code_configs, app_utils

#========================================================================================

# Global Variables - only global to this file
g_Date    = "na"
g_Owner   = "na"
g_Mission = "na"

g_OutDir = "."

g_Tbls = []

#========================================================================================

def inits():
    global g_OutDir, g_Mission, g_Date, g_Owner, g_Tbls

    g_Date = app_code_configs.get_date()
    g_Owner = app_code_configs.get_owner()
    g_Mission = app_code_configs.get_mission()
    g_OutDir = app_code_configs.get_outdir()

    g_Tbls.extend(app_code_configs.get_tables())

#========================================================================================

def construct_msgheader_content(tgtApp):
    global g_Mission, g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    # Construct file description
    head = """\
/*=======================================================================================
** File Name:  %s_msg.h
**
** Title:  Message Definition Header File for %s Application
**
** $Author:    %s
** $Revision: 1.1 $
** $Date:      %s
**
** Purpose:  To define %s's command and telemetry message defintions 
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   %s | %s | Build #: Code Started
**
**=====================================================================================*/
    """ % (lcApp, ucApp, g_Owner, g_Date, ucApp, g_Date, g_Owner)

    # Construct file content
    tmpStr = ucApp + "_MSG_H"

    part1 = """
#ifndef _%s_
#define _%s_

/*
** Pragmas
*/

/*
** Include Files
*/

""" % (tmpStr, tmpStr)

    part2 = ""
    if "iload" in g_Tbls:
        part2 = """ 
#include \"%s_iload_utils.h\"""" % (lcApp)

    part3 = ""
    if "cds" in g_Tbls:
        part3 = """
#include \"%s_cds_utils.h\"""" % (lcApp)

    part4 = """

/*
** Local Defines
*/

/*
** %s command codes
*/
#define %s_NOOP_CC                 0
#define %s_RESET_CC                1

/*
** Local Structure Declarations
*/
typedef struct
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8              usCmdCnt;
    uint8              usCmdErrCnt;

    /* TODO:  Add declarations for additional housekeeping data here */

} %s_HkTlm_t;


#endif /* _%s_ */

/*=======================================================================================
** End of file %s_msg.h
**=====================================================================================*/
    """ % (ucApp, ucApp, ucApp, ucApp, tmpStr, lcApp)

    content = head + part1 + part2 + part3 + part4

    return content

#========================================================================================

def construct_header_content(tgtApp):
    global g_Mission, g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    # Construct file description
    head = """\
/*=======================================================================================
** File Name:  %s_app.h
**
** Title:  Header File for %s Application
**
** $Author:    %s
** $Revision: 1.1 $
** $Date:      %s
**
** Purpose:  To define %s's internal macros, data types, global variables and
**           function prototypes
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   %s | %s | Build #: Code Started
**
**=====================================================================================*/
    """ % (lcApp, ucApp, g_Owner, g_Date, ucApp, g_Date, g_Owner)

    # Construct file content
    tmpStr = ucApp + "_APP_H"

    part1 = """
#ifndef _%s_
#define _%s_

/*
** Pragmas
*/

/*
** Include Files
*/
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include \"%s_platform_cfg.h\"
#include \"%s_mission_cfg.h\"
#include \"%s_private_ids.h\"
#include \"%s_private_types.h\"
#include \"%s_perfids.h\"
#include \"%s_msgids.h\"
#include \"%s_msg.h\"

""" % (tmpStr, tmpStr, lcApp, lcApp, lcApp, lcApp, lcApp, lcApp, lcApp)

    part2 = ""
    if "iload" in g_Tbls:
        part2 = """ 
#include \"%s_iload_utils.h\"""" % (lcApp)

    part3 = ""
    if "cds" in g_Tbls:
        part3 = """
#include \"%s_cds_utils.h\"""" % (lcApp)

    part4 = """

/*
** Local Defines
*/
#define %s_TIMEOUT_MSEC    1000

/*
** Local Structure Declarations
*/
typedef struct
{
    /* CFE Event table */
    CFE_EVS_BinFilter_t  EventTbl[%s_EVT_CNT];

    /* CFE scheduling pipe */
    CFE_SB_PipeId_t  SchPipeId; 
    uint16           usSchPipeDepth;
    char             cSchPipeName[OS_MAX_API_NAME];

    /* CFE command pipe */
    CFE_SB_PipeId_t  CmdPipeId;
    uint16           usCmdPipeDepth;
    char             cCmdPipeName[OS_MAX_API_NAME];
    
    /* CFE telemetry pipe */
    CFE_SB_PipeId_t  TlmPipeId;
    uint16           usTlmPipeDepth;
    char             cTlmPipeName[OS_MAX_API_NAME];

    /* Task-related */
    uint32  uiRunStatus;
    """ % (ucApp, ucApp)

    part5 = ""
    if "iload" in g_Tbls:
        part5 = """
    /* ILoad table-related */
    CFE_TBL_Handle_t  ILoadTblHdl;
    %s_ILoadTblEntry_t*  ILoadTblPtr;
    """ % (ucApp)

    part6 = ""
    if "cds" in g_Tbls:
        part6 = """
    /* Critical Data Storage (CDS) table-related */
    CFE_ES_CDSHandle_t  CdsTblHdl;
    %s_CdsTbl_t  CdsTbl;
    """ % (ucApp)

    part7 = """
    /* Input data - from I/O devices or subscribed from other apps' output data.
       Data structure should be defined in %s/fsw/src/%s_private_types.h */
    %s_InData_t   InData;

    /* Output data - to be published at the end of a Wakeup cycle.
       Data structure should be defined in %s/fsw/src/%s_private_types.h */
    %s_OutData_t  OutData;

    /* Housekeeping telemetry - for downlink only.
       Data structure should be defined in %s/fsw/src/%s_msg.h */
    %s_HkTlm_t  HkTlm;

    /* TODO:  Add declarations for additional private data here */
} %s_AppData_t;

/*
** External Global Variables
*/

/*
** Global Variables
*/

/*
** Local Variables
*/

/*
** Local Function Prototypes
*/
int32  %s_InitApp(void);
int32  %s_InitEvent(void);
int32  %s_InitData(void);
int32  %s_InitPipe(void);

void  %s_AppMain(void);

void  %s_CleanupCallback(void);

int32  %s_RcvMsg(int32 iBlocking);

void  %s_ProcessNewData(void);
void  %s_ProcessNewCmds(void);
void  %s_ProcessNewAppCmds(CFE_SB_Msg_t*);

void  %s_ReportHousekeeping(void);
void  %s_SendOutData(void);

boolean  %s_VerifyCmdLength(CFE_SB_Msg_t*, uint16);

#endif /* _%s_ */

/*=======================================================================================
** End of file %s_app.h
**=====================================================================================*/
    """ % (lcApp, lcApp, ucApp, lcApp, lcApp, ucApp, lcApp, lcApp, \
           ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp,  \
           ucApp, ucApp, ucApp, ucApp, ucApp, tmpStr, lcApp)

    content = head + part1 + part2 + part3 + part4 + part5 + part6 + part7

    return content

#========================================================================================

def construct_source_head_content(tgtApp):
    global g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    head = """\
/*=======================================================================================
** File Name:  %s_app.c
**
** Title:  Function Definitions for %s Application
**
** $Author:    %s
** $Revision: 1.1 $
** $Date:      %s
**
** Purpose:  This source file contains all necessary function definitions to run %s
**           application.
**
** Functions Defined:
**    Function X - Brief purpose of function X
**    Function Y - Brief purpose of function Y
**    Function Z - Brief purpose of function Z
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to all functions in the file.
**    2. List the external source(s) and event(s) that can cause the funcs in this
**       file to execute.
**    3. List known limitations that apply to the funcs in this file.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   %s | %s | Build #: Code Started
**
**=====================================================================================*/

/*
** Pragmas
*/

/*
** Include Files
*/
#include <string.h>

#include \"cfe.h\"

#include \"%s_platform_cfg.h\"
#include \"%s_mission_cfg.h\"
#include \"%s_app.h\"

/*
** Local Defines
*/

/*
** Local Structure Declarations
*/

/*
** External Global Variables
*/

/*
** Global Variables
*/
%s_AppData_t  g_%s_AppData;

/*
** Local Variables
*/

/*
** Local Function Definitions
*/
    """ % (lcApp, ucApp, g_Owner, g_Date, ucApp, g_Date, g_Owner, lcApp, \
           lcApp, lcApp, ucApp, ucApp)

    return head

#========================================================================================

def construct_source_init_app_content(tgtApp):
    global g_Owner, g_Date, g_Tbls

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    part1 = """
/*=====================================================================================
** Name: %s_InitApp
**
** Purpose: To initialize all data local to and used by %s application
**
** Arguments:
**    None
**
** Returns:
**    int32 iStatus - Status of initialization
**
** Routines Called:
**    CFE_ES_RegisterApp
**    CFE_ES_WriteToSysLog
**    CFE_EVS_SendEvent
**    OS_TaskInstallDeleteHandler
**    %s_InitEvent
**    %s_InitPipe
**    %s_InitData""" % (ucApp, ucApp, ucApp, ucApp, ucApp)

    part2 = ""
    if "iload-table" in g_Tbls:
        part2 = """
**    %s_ILoadInit""" % (ucApp)

    part3 = ""
    if "cds-table" in g_Tbls:
        part3 = """
**    %s_CdsInit""" % (ucApp)

    part4 = """
**
** Called By:
**    %s_AppMain
**
** Global Inputs/Reads:
**    TBD
**
** Global Outputs/Writes:
**    TBD
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  %s 
**
** History:  Date Written  %s
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 %s_InitApp()
{
    int32  iStatus=CFE_SUCCESS;

    g_%s_AppData.uiRunStatus = CFE_ES_APP_RUN;

    iStatus = CFE_ES_RegisterApp();
    if (iStatus != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog(\"%s - Failed to register the app (0x%%08X)\\n\", iStatus);
        goto %s_InitApp_Exit_Tag;
    }

    if ((%s_InitEvent() != CFE_SUCCESS) || 
        (%s_InitPipe() != CFE_SUCCESS) || 
        (%s_InitData() != CFE_SUCCESS)""" \
    % (ucApp, g_Owner, g_Date, ucApp, ucApp, ucApp, ucApp, ucApp, \
       ucApp, ucApp)

    part5 = ""
    if "iload" in g_Tbls:
        part5 = """ ||
        (%s_InitILoadTbl() != CFE_SUCCESS)""" % (ucApp)

    part6 = ""
    if "cds" in g_Tbls:
        part6 = """ ||
        (%s_InitCdsTbl() != CFE_SUCCESS)""" % (ucApp)

    part7 = """)
    {
        iStatus = -1;
        goto %s_InitApp_Exit_Tag;
    }

    /* Install the cleanup callback */
    OS_TaskInstallDeleteHandler((void*)&%s_CleanupCallback);

%s_InitApp_Exit_Tag:
    if (iStatus == CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(%s_INIT_INF_EID, CFE_EVS_INFORMATION,
                          \"%s - Application initialized\");
    }
    else
    {
        CFE_ES_WriteToSysLog(\"%s - Application failed to initialize\\n\");
    }

    return (iStatus);
}
    """ % (ucApp, ucApp, ucApp, ucApp, ucApp, ucApp)

    content = part1 + part2 + part3 + part4 + part5 + part6 + part7

    return content

#========================================================================================

def construct_source_init_event_content(tgtApp):
    global g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    content = """
/*=====================================================================================
** Name: %s_InitEvent
**
** Purpose: To initialize and register event table for %s application
**
** Arguments:
**    None
**
** Returns:
**    int32 iStatus - Status of initialization
**
** Routines Called:
**    CFE_EVS_Register
**    CFE_ES_WriteToSysLog
**
** Called By:
**    %s_InitApp
**
** Global Inputs/Reads:
**    TBD
**
** Global Outputs/Writes:
**    g_%s_AppData.EventTbl
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  %s 
**
** History:  Date Written  %s
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 %s_InitEvent()
{
    int32  iStatus=CFE_SUCCESS;

    /* Create the event table */
    memset((void*)g_%s_AppData.EventTbl, 0x00, sizeof(g_%s_AppData.EventTbl));

    g_%s_AppData.EventTbl[0].EventID = %s_RESERVED_EID;
    g_%s_AppData.EventTbl[1].EventID = %s_INF_EID;
    g_%s_AppData.EventTbl[2].EventID = %s_INIT_INF_EID;
    g_%s_AppData.EventTbl[3].EventID = %s_ILOAD_INF_EID;
    g_%s_AppData.EventTbl[4].EventID = %s_CDS_INF_EID;
    g_%s_AppData.EventTbl[5].EventID = %s_CMD_INF_EID;

    g_%s_AppData.EventTbl[ 6].EventID = %s_ERR_EID;
    g_%s_AppData.EventTbl[ 7].EventID = %s_INIT_ERR_EID;
    g_%s_AppData.EventTbl[ 8].EventID = %s_ILOAD_ERR_EID;
    g_%s_AppData.EventTbl[ 9].EventID = %s_CDS_ERR_EID;
    g_%s_AppData.EventTbl[10].EventID = %s_CMD_ERR_EID;
    g_%s_AppData.EventTbl[11].EventID = %s_PIPE_ERR_EID;
    g_%s_AppData.EventTbl[12].EventID = %s_MSGID_ERR_EID;
    g_%s_AppData.EventTbl[13].EventID = %s_MSGLEN_ERR_EID;

    /* Register the table with CFE */
    iStatus = CFE_EVS_Register(g_%s_AppData.EventTbl,
                               %s_EVT_CNT, CFE_EVS_BINARY_FILTER);
    if (iStatus != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog(\"%s - Failed to register with EVS (0x%%08X)\\n\", iStatus);
    }

    return (iStatus);
}
    """ % (ucApp, ucApp, ucApp, ucApp, g_Owner, g_Date, ucApp, \
           ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, \
           ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, \
           ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, \
           ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, \
           ucApp, ucApp, ucApp, ucApp, ucApp)

    return content

#========================================================================================

def construct_source_init_pipe_content(tgtApp):
    global g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    content = """
/*=====================================================================================
** Name: %s_InitPipe
**
** Purpose: To initialize all message pipes and subscribe to messages for %s application
**
** Arguments:
**    None
**
** Returns:
**    int32 iStatus - Status of initialization
**
** Routines Called:
**    CFE_SB_CreatePipe
**    CFE_SB_Subscribe
**    CFE_ES_WriteToSysLog
**
** Called By:
**    %s_InitApp
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    g_%s_AppData.usSchPipeDepth
**    g_%s_AppData.cSchPipeName
**    g_%s_AppData.SchPipeId
**    g_%s_AppData.usCmdPipeDepth
**    g_%s_AppData.cCmdPipeName
**    g_%s_AppData.CmdPipeId
**    g_%s_AppData.usTlmPipeDepth
**    g_%s_AppData.cTlmPipeName
**    g_%s_AppData.TlmPipeId
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  %s 
**
** History:  Date Written  %s
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 %s_InitPipe()
{
    int32  iStatus=CFE_SUCCESS;

    /* Init schedule pipe */
    g_%s_AppData.usSchPipeDepth = %s_SCH_PIPE_DEPTH;
    memset((void*)g_%s_AppData.cSchPipeName, '\\0', sizeof(g_%s_AppData.cSchPipeName));
    strncpy(g_%s_AppData.cSchPipeName, \"%s_SCH_PIPE\", OS_MAX_API_NAME-1);

    /* Subscribe to Wakeup messages */
    iStatus = CFE_SB_CreatePipe(&g_%s_AppData.SchPipeId,
                                 g_%s_AppData.usSchPipeDepth,
                                 g_%s_AppData.cSchPipeName);
    if (iStatus == CFE_SUCCESS)
    {
        iStatus = CFE_SB_SubscribeEx(%s_WAKEUP_MID, g_%s_AppData.SchPipeId, CFE_SB_Default_Qos, 1);

        if (iStatus != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog(\"%s - Sch Pipe failed to subscribe to %s_WAKEUP_MID. (0x%%08X)\\n\", iStatus);
            goto %s_InitPipe_Exit_Tag;
        }
        
    }
    else
    {
        CFE_ES_WriteToSysLog(\"%s - Failed to create SCH pipe (0x%%08X)\\n\", iStatus);
        goto %s_InitPipe_Exit_Tag;
    }

    /* Init command pipe */
    g_%s_AppData.usCmdPipeDepth = %s_CMD_PIPE_DEPTH ;
    memset((void*)g_%s_AppData.cCmdPipeName, '\\0', sizeof(g_%s_AppData.cCmdPipeName));
    strncpy(g_%s_AppData.cCmdPipeName, \"%s_CMD_PIPE\", OS_MAX_API_NAME-1);

    /* Subscribe to command messages */
    iStatus = CFE_SB_CreatePipe(&g_%s_AppData.CmdPipeId,
                                 g_%s_AppData.usCmdPipeDepth,
                                 g_%s_AppData.cCmdPipeName);
    if (iStatus == CFE_SUCCESS)
    {
        /* Subscribe to command messages */
        iStatus = CFE_SB_Subscribe(%s_CMD_MID, g_%s_AppData.CmdPipeId);

        if (iStatus != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog(\"%s - CMD Pipe failed to subscribe to %s_CMD_MID. (0x%%08X)\\n\", iStatus);
            goto %s_InitPipe_Exit_Tag;
        }

        iStatus = CFE_SB_Subscribe(%s_SEND_HK_MID, g_%s_AppData.CmdPipeId);

        if (iStatus != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog(\"%s - CMD Pipe failed to subscribe to %s_SEND_HK_MID. (0x%%08X)\\n\", iStatus);
            goto %s_InitPipe_Exit_Tag;
        }
        
    }
    else
    {
        CFE_ES_WriteToSysLog(\"%s - Failed to create CMD pipe (0x%%08X)\\n\", iStatus);
        goto %s_InitPipe_Exit_Tag;
    }

    /* Init telemetry pipe */
    g_%s_AppData.usTlmPipeDepth = %s_TLM_PIPE_DEPTH;
    memset((void*)g_%s_AppData.cTlmPipeName, '\\0', sizeof(g_%s_AppData.cTlmPipeName));
    strncpy(g_%s_AppData.cTlmPipeName, \"%s_TLM_PIPE\", OS_MAX_API_NAME-1);

    /* Subscribe to telemetry messages on the telemetry pipe */
    iStatus = CFE_SB_CreatePipe(&g_%s_AppData.TlmPipeId,
                                 g_%s_AppData.usTlmPipeDepth,
                                 g_%s_AppData.cTlmPipeName);
    if (iStatus == CFE_SUCCESS)
    {
        /* TODO:  Add CFE_SB_Subscribe() calls for other apps' output data here.
        **
        ** Examples:
        **     CFE_SB_Subscribe(GNCEXEC_OUT_DATA_MID, g_%s_AppData.TlmPipeId);
        */
    }
    else
    {
        CFE_ES_WriteToSysLog(\"%s - Failed to create TLM pipe (0x%%08X)\\n\", iStatus);
        goto %s_InitPipe_Exit_Tag;
    }

%s_InitPipe_Exit_Tag:
    return (iStatus);
}
    """ % (ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, \
           ucApp, ucApp, ucApp, ucApp, ucApp, g_Owner, g_Date, \
           ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp,\
           ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp,\
           ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp,\
           ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, \
           ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, \
           ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp)

    return content

#========================================================================================

def construct_source_init_data_content(tgtApp):
    global g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    content = """
/*=====================================================================================
** Name: %s_InitData
**
** Purpose: To initialize global variables used by %s application
**
** Arguments:
**    None
**
** Returns:
**    int32 iStatus - Status of initialization
**
** Routines Called:
**    CFE_SB_InitMsg
**
** Called By:
**    %s_InitApp
**
** Global Inputs/Reads:
**    TBD
**
** Global Outputs/Writes:
**    g_%s_AppData.InData
**    g_%s_AppData.OutData
**    g_%s_AppData.HkTlm
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  %s 
**
** History:  Date Written  %s
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 %s_InitData()
{
    int32  iStatus=CFE_SUCCESS;

    /* Init input data */
    memset((void*)&g_%s_AppData.InData, 0x00, sizeof(g_%s_AppData.InData));

    /* Init output data */
    memset((void*)&g_%s_AppData.OutData, 0x00, sizeof(g_%s_AppData.OutData));
    CFE_SB_InitMsg(&g_%s_AppData.OutData,
                   %s_OUT_DATA_MID, sizeof(g_%s_AppData.OutData), TRUE);

    /* Init housekeeping packet */
    memset((void*)&g_%s_AppData.HkTlm, 0x00, sizeof(g_%s_AppData.HkTlm));
    CFE_SB_InitMsg(&g_%s_AppData.HkTlm,
                   %s_HK_TLM_MID, sizeof(g_%s_AppData.HkTlm), TRUE);

    return (iStatus);
}
    """ % (ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, g_Owner, \
           g_Date, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, \
           ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp)

    return content

#========================================================================================

def construct_source_main_content(tgtApp):
    global g_Owner, g_Date, g_Tbls

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    part1 = """
/*=====================================================================================
** Name: %s_AppMain
**
** Purpose: To define %s application's entry point and main process loop
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**    CFE_ES_RegisterApp
**    CFE_ES_RunLoop
**    CFE_ES_PerfLogEntry
**    CFE_ES_PerfLogExit
**    CFE_ES_ExitApp
**    CFE_ES_WaitForStartupSync
**    %s_InitApp
**    %s_RcvMsg
**
** Called By:
**    TBD
**
** Global Inputs/Reads:
**    TBD
**
** Global Outputs/Writes:
**    TBD
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  %s 
**
** History:  Date Written  %s
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void %s_AppMain()
{
    /* Register the application with Executive Services */
    CFE_ES_RegisterApp();

    /* Start Performance Log entry */
    CFE_ES_PerfLogEntry(%s_MAIN_TASK_PERF_ID);

    /* Perform application initializations */
    if (%s_InitApp() != CFE_SUCCESS)
    {
        g_%s_AppData.uiRunStatus = CFE_ES_APP_ERROR;
    } else {
        /* Do not perform performance monitoring on startup sync */
        CFE_ES_PerfLogExit(%s_MAIN_TASK_PERF_ID);
        CFE_ES_WaitForStartupSync(%s_TIMEOUT_MSEC);
        CFE_ES_PerfLogEntry(%s_MAIN_TASK_PERF_ID);
    }

    /* Application main loop */
    while (CFE_ES_RunLoop(&g_%s_AppData.uiRunStatus) == TRUE)
    {
        %s_RcvMsg(CFE_SB_PEND_FOREVER);""" \
    % (ucApp, ucApp, ucApp, ucApp, g_Owner, g_Date, ucApp, ucApp, ucApp, ucApp, \
       ucApp, ucApp, ucApp, ucApp, ucApp)

    part2 = ""
    if "cds" in g_Tbls:
        part2 = """

        /* This is only a suggestion for when to update and save CDS table.
        ** Depends on the nature of the application, the frequency of update
        ** and save can be more or less independently.
        */

        /* Start Performance Log entry */
        CFE_ES_PerfLogEntry(%s_MAIN_TASK_PERF_ID);

        %s_UpdateCdsTbl();
        %s_SaveCdsTbl();

        /* Stop Performance Log entry */
        CFE_ES_PerfLogExit(%s_MAIN_TASK_PERF_ID); """ % (ucApp, ucApp, ucApp, ucApp)

    part3 = """
    }

    /* Stop Performance Log entry */
    CFE_ES_PerfLogExit(%s_MAIN_TASK_PERF_ID);

    /* Exit the application */
    CFE_ES_ExitApp(g_%s_AppData.uiRunStatus);
} 
    """ % (ucApp, ucApp)

    content = part1 + part2 + part3

    return content

#========================================================================================

def construct_source_cleanup_content(tgtApp):
    global g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    content = """
/*=====================================================================================
** Name: %s_CleanupCallback
**
** Purpose: To handle any neccesary cleanup prior to application exit
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**    TBD
**
** Called By:
**    TBD
**
** Global Inputs/Reads:
**    TBD
**
** Global Outputs/Writes:
**    TBD
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  %s 
**
** History:  Date Written  %s
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void %s_CleanupCallback()
{
    /* TODO:  Add code to cleanup memory and other cleanup here */
}
    """ % (ucApp, g_Owner, g_Date, ucApp)

    return content

#========================================================================================

def construct_source_recv_msg_content(tgtApp):
    global g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    content = """
/*=====================================================================================
** Name: %s_RcvMsg
**
** Purpose: To receive and process messages for %s application
**
** Arguments:
**    None
**
** Returns:
**    int32 iStatus - Status of initialization 
**
** Routines Called:
**    CFE_SB_RcvMsg
**    CFE_SB_GetMsgId
**    CFE_EVS_SendEvent
**    CFE_ES_PerfLogEntry
**    CFE_ES_PerfLogExit
**    %s_ProcessNewCmds
**    %s_ProcessNewData
**    %s_SendOutData
**
** Called By:
**    %s_Main
**
** Global Inputs/Reads:
**    g_%s_AppData.SchPipeId
**
** Global Outputs/Writes:
**    g_%s_AppData.uiRunStatus
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  %s 
**
** History:  Date Written  %s
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 %s_RcvMsg(int32 iBlocking)
{
    int32           iStatus=CFE_SUCCESS;
    CFE_SB_Msg_t*   MsgPtr=NULL;
    CFE_SB_MsgId_t  MsgId;

    /* Stop Performance Log entry */
    CFE_ES_PerfLogExit(%s_MAIN_TASK_PERF_ID);

    /* Wait for WakeUp messages from scheduler */
    iStatus = CFE_SB_RcvMsg(&MsgPtr, g_%s_AppData.SchPipeId, iBlocking);

    /* Start Performance Log entry */
    CFE_ES_PerfLogEntry(%s_MAIN_TASK_PERF_ID);

    if (iStatus == CFE_SUCCESS)
    {
        MsgId = CFE_SB_GetMsgId(MsgPtr);
        switch (MsgId)
	{
            case %s_WAKEUP_MID:
                %s_ProcessNewCmds();
                %s_ProcessNewData();

                /* TODO:  Add more code here to handle other things when app wakes up */

                /* The last thing to do at the end of this Wakeup cycle should be to
                   automatically publish new output. */
                %s_SendOutData();
                break;

            default:
                CFE_EVS_SendEvent(%s_MSGID_ERR_EID, CFE_EVS_ERROR,
                                  \"%s - Recvd invalid SCH msgId (0x%%08X)\", MsgId);
        }
    }
    else if (iStatus == CFE_SB_NO_MESSAGE)
    {
        /* If there's no incoming message, you can do something here, or nothing */
    }
    else
    {
        /* This is an example of exiting on an error.
        ** Note that a SB read error is not always going to result in an app quitting.
        */
        CFE_EVS_SendEvent(%s_PIPE_ERR_EID, CFE_EVS_ERROR,
			  \"%s: SB pipe read error (0x%%08X), app will exit\", iStatus);
        g_%s_AppData.uiRunStatus= CFE_ES_APP_ERROR;
    }

    return (iStatus);
}
    """ % (ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, g_Owner, g_Date, \
           ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, \
           ucApp, ucApp)

    return content

#========================================================================================

def construct_source_process_new_data_content(tgtApp):
    global g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    content = """
/*=====================================================================================
** Name: %s_ProcessNewData
**
** Purpose: To process incoming data subscribed by %s application
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**    CFE_SB_RcvMsg
**    CFE_SB_GetMsgId
**    CFE_EVS_SendEvent
**
** Called By:
**    %s_RcvMsg
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    None
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  %s 
**
** History:  Date Written  %s
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void %s_ProcessNewData()
{
    int iStatus = CFE_SUCCESS;
    CFE_SB_Msg_t*   TlmMsgPtr=NULL;
    CFE_SB_MsgId_t  TlmMsgId;

    /* Process telemetry messages till the pipe is empty */
    while (1)
    {
        iStatus = CFE_SB_RcvMsg(&TlmMsgPtr, g_%s_AppData.TlmPipeId, CFE_SB_POLL);
        if (iStatus == CFE_SUCCESS)
        {
            TlmMsgId = CFE_SB_GetMsgId(TlmMsgPtr);
            switch (TlmMsgId)
            {
                /* TODO:  Add code to process all subscribed data here 
                **
                ** Example:
                **     case NAV_OUT_DATA_MID:
                **         %s_ProcessNavData(TlmMsgPtr);
                **         break;
                */

                default:
                    CFE_EVS_SendEvent(%s_MSGID_ERR_EID, CFE_EVS_ERROR,
                                      \"%s - Recvd invalid TLM msgId (0x%%08X)\", TlmMsgId);
                    break;
            }
        }
        else if (iStatus == CFE_SB_NO_MESSAGE)
        {
            break;
        }
        else
        {
            CFE_EVS_SendEvent(%s_PIPE_ERR_EID, CFE_EVS_ERROR,
                  \"%s: CMD pipe read error (0x%%08X)\", iStatus);
            g_%s_AppData.uiRunStatus = CFE_ES_APP_ERROR;
            break;
        }
    }
}
    """ % (ucApp, ucApp, ucApp, g_Owner, g_Date, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp)

    return content

#========================================================================================

def construct_source_process_new_cmds_content(tgtApp):
    global g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    content = """
/*=====================================================================================
** Name: %s_ProcessNewCmds
**
** Purpose: To process incoming command messages for %s application
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**    CFE_SB_RcvMsg
**    CFE_SB_GetMsgId
**    CFE_EVS_SendEvent
**    %s_ProcessNewAppCmds
**    %s_ReportHousekeeping
**
** Called By:
**    %s_RcvMsg
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    None
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  %s 
**
** History:  Date Written  %s
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void %s_ProcessNewCmds()
{
    int iStatus = CFE_SUCCESS;
    CFE_SB_Msg_t*   CmdMsgPtr=NULL;
    CFE_SB_MsgId_t  CmdMsgId;

    /* Process command messages till the pipe is empty */
    while (1)
    {
        iStatus = CFE_SB_RcvMsg(&CmdMsgPtr, g_%s_AppData.CmdPipeId, CFE_SB_POLL);
        if(iStatus == CFE_SUCCESS)
        {
            CmdMsgId = CFE_SB_GetMsgId(CmdMsgPtr);
            switch (CmdMsgId)
            {
                case %s_CMD_MID:
                    %s_ProcessNewAppCmds(CmdMsgPtr);
                    break;

                case %s_SEND_HK_MID:
                    %s_ReportHousekeeping();
                    break;

                /* TODO:  Add code to process other subscribed commands here
                **
                ** Example:
                **     case CFE_TIME_DATA_CMD_MID:
                **         %s_ProcessTimeDataCmd(CmdMsgPtr);
                **         break;
                */

                default:
                    CFE_EVS_SendEvent(%s_MSGID_ERR_EID, CFE_EVS_ERROR,
                                      \"%s - Recvd invalid CMD msgId (0x%%08X)\", CmdMsgId);
                    break;
            }
        }
        else if (iStatus == CFE_SB_NO_MESSAGE)
        {
            break;
        }
        else
        {
            CFE_EVS_SendEvent(%s_PIPE_ERR_EID, CFE_EVS_ERROR,
                  \"%s: CMD pipe read error (0x%%08X)\", iStatus);
            g_%s_AppData.uiRunStatus = CFE_ES_APP_ERROR;
            break;
        }
    }
}
    """ % (ucApp, ucApp, ucApp, ucApp, ucApp, g_Owner, g_Date, ucApp, ucApp, ucApp, ucApp, \
           ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp)

    return content

#========================================================================================

def construct_source_process_new_app_cmds_content(tgtApp):
    global g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    content = """
/*=====================================================================================
** Name: %s_ProcessNewAppCmds
**
** Purpose: To process command messages targeting %s application
**
** Arguments:
**    CFE_SB_Msg_t*  MsgPtr - new command message pointer
**
** Returns:
**    None
**
** Routines Called:
**    CFE_SB_GetCmdCode
**    CFE_EVS_SendEvent
**
** Called By:
**    %s_ProcessNewCmds
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    g_%s_AppData.HkTlm.usCmdCnt
**    g_%s_AppData.HkTlm.usCmdErrCnt
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  %s 
**
** History:  Date Written  %s
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void %s_ProcessNewAppCmds(CFE_SB_Msg_t* MsgPtr)
{
    uint32  uiCmdCode=0;

    if (MsgPtr != NULL)
    {
        uiCmdCode = CFE_SB_GetCmdCode(MsgPtr);
        switch (uiCmdCode)
        {
            case %s_NOOP_CC:
                g_%s_AppData.HkTlm.usCmdCnt++;
                CFE_EVS_SendEvent(%s_CMD_INF_EID, CFE_EVS_INFORMATION,
                                  \"%s - Recvd NOOP cmd (%%d)\", uiCmdCode);
                break;

            case %s_RESET_CC:
                g_%s_AppData.HkTlm.usCmdCnt = 0;
                g_%s_AppData.HkTlm.usCmdErrCnt = 0;
                CFE_EVS_SendEvent(%s_CMD_INF_EID, CFE_EVS_INFORMATION,
                                  \"%s - Recvd RESET cmd (%%d)\", uiCmdCode);
                break;

            /* TODO:  Add code to process the rest of the %s commands here */

            default:
                g_%s_AppData.HkTlm.usCmdErrCnt++;
                CFE_EVS_SendEvent(%s_MSGID_ERR_EID, CFE_EVS_ERROR,
                                  \"%s - Recvd invalid cmdId (%%d)", uiCmdCode);
                break;
        }
    }
}
    """ % (ucApp, ucApp, ucApp, ucApp, ucApp, g_Owner, g_Date, ucApp, ucApp, ucApp, \
           ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp)

    return content

#========================================================================================

def construct_source_report_hk_content(tgtApp):
    global g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    content = """
/*=====================================================================================
** Name: %s_ReportHousekeeping
**
** Purpose: To send housekeeping message
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**    TBD
**
** Called By:
**    %s_ProcessNewCmds
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    TBD
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  GSFC, %s
**
** History:  Date Written  %s
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void %s_ReportHousekeeping()
{
    /* TODO:  Add code to update housekeeping data, if needed, here.  */

    CFE_SB_TimeStampMsg((CFE_SB_Msg_t*)&g_%s_AppData.HkTlm);
    CFE_SB_SendMsg((CFE_SB_Msg_t*)&g_%s_AppData.HkTlm);
}
    """ % (ucApp, ucApp, g_Owner, g_Date, ucApp, ucApp, ucApp)

    return content

#========================================================================================

def construct_source_send_out_data_content(tgtApp):
    global g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    content = """
/*=====================================================================================
** Name: %s_SendOutData
**
** Purpose: To publish 1-Wakeup cycle output data
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**    TBD
**
** Called By:
**    %s_RcvMsg
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    TBD
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  %s
**
** History:  Date Written  %s
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void %s_SendOutData()
{
    /* TODO:  Add code to update output data, if needed, here.  */

    CFE_SB_TimeStampMsg((CFE_SB_Msg_t*)&g_%s_AppData.OutData);
    CFE_SB_SendMsg((CFE_SB_Msg_t*)&g_%s_AppData.OutData);
}
    """ % (ucApp, ucApp, g_Owner, g_Date, ucApp, ucApp, ucApp)

    return content

#========================================================================================

def construct_source_verify_cmd_len_content(tgtApp):
    global g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    content = """
/*=====================================================================================
** Name: %s_VerifyCmdLength
**
** Purpose: To verify command length for a particular command message
**
** Arguments:
**    CFE_SB_Msg_t*  MsgPtr      - command message pointer
**    uint16         usExpLength - expected command length
**
** Returns:
**    boolean bResult - result of verification
**
** Routines Called:
**    TBD
**
** Called By:
**    %s_ProcessNewCmds
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    TBD
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  %s 
**
** History:  Date Written  %s
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
boolean %s_VerifyCmdLength(CFE_SB_Msg_t* MsgPtr,
                           uint16 usExpectedLen)
{
    boolean bResult=FALSE;
    uint16  usMsgLen=0;

    if (MsgPtr != NULL)
    {
        usMsgLen = CFE_SB_GetTotalMsgLength(MsgPtr);

        if (usExpectedLen != usMsgLen)
        {
            CFE_SB_MsgId_t MsgId = CFE_SB_GetMsgId(MsgPtr);
            uint16 usCmdCode = CFE_SB_GetCmdCode(MsgPtr);

            CFE_EVS_SendEvent(%s_MSGLEN_ERR_EID, CFE_EVS_ERROR,
                              \"%s - Rcvd invalid msgLen: msgId=0x%%08X, cmdCode=%%d, \"
                              \"msgLen=%%d, expectedLen=%%d\",
                              MsgId, usCmdCode, usMsgLen, usExpectedLen);
            g_%s_AppData.HkTlm.usCmdErrCnt++;
        }
    }

    return (bResult);
}
    """ % (ucApp, ucApp, g_Owner, g_Date, ucApp, ucApp, ucApp, ucApp)

    return content

#========================================================================================

def construct_source_tail_content(tgtApp):
    content = """
/*=======================================================================================
** End of file %s_app.c
**=====================================================================================*/
    """ % (tgtApp.lower())

    return content

#========================================================================================

def generate_msgheader(tgtApp):
    global g_OutDir

    # Construct file path
    name = tgtApp.lower() + "_msg.h"
    filePath = os.path.join(g_OutDir, tgtApp.lower(), "fsw", "src", name)
    fileExist_p = os.path.exists(filePath)
    # Open file
    fileHdl = app_utils.open_file_for_writing(filePath, False)

    # Write to file
    fileHdl.write(construct_msgheader_content(tgtApp)) 

    # Close file
    app_utils.close_file_from_writing(fileHdl, filePath, fileExist_p)

#========================================================================================

def generate_header(tgtApp):
    global g_OutDir

    # Construct file path
    name = tgtApp.lower() + "_app.h"
    filePath = os.path.join(g_OutDir, tgtApp.lower(), "fsw", "src", name)
    fileExist_p = os.path.exists(filePath)
    # Open file
    fileHdl = app_utils.open_file_for_writing(filePath, False)

    # Write to file
    fileHdl.write(construct_header_content(tgtApp))

    # Close file
    app_utils.close_file_from_writing(fileHdl, filePath, fileExist_p)

#========================================================================================

def generate_source(tgtApp):
    global g_OutDir

    # Construct file path
    name = tgtApp.lower() + "_app.c"
    filePath = os.path.join(g_OutDir, tgtApp.lower(), "fsw", "src", name)
    fileExist_p = os.path.exists(filePath)
    # Open file
    fileHdl = app_utils.open_file_for_writing(filePath, False)

    # Write to file
    fileHdl.write(construct_source_head_content(tgtApp))
    fileHdl.write(construct_source_init_event_content(tgtApp))
    fileHdl.write(construct_source_init_pipe_content(tgtApp))
    fileHdl.write(construct_source_init_data_content(tgtApp))
    fileHdl.write(construct_source_init_app_content(tgtApp))
    fileHdl.write(construct_source_cleanup_content(tgtApp))
    fileHdl.write(construct_source_recv_msg_content(tgtApp))
    fileHdl.write(construct_source_process_new_data_content(tgtApp))
    fileHdl.write(construct_source_process_new_cmds_content(tgtApp))
    fileHdl.write(construct_source_process_new_app_cmds_content(tgtApp))
    fileHdl.write(construct_source_report_hk_content(tgtApp))
    fileHdl.write(construct_source_send_out_data_content(tgtApp))
    fileHdl.write(construct_source_verify_cmd_len_content(tgtApp))
    fileHdl.write(construct_source_main_content(tgtApp))
    fileHdl.write(construct_source_tail_content(tgtApp))

    # Close file
    app_utils.close_file_from_writing(fileHdl, filePath, fileExist_p)

#========================================================================================
# End of app_code_src.py
#========================================================================================

