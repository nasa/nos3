#========================================================================================
# File:    app_code_tables.py
# Author:  Tam Ngo/JSC
# Date:    2012-02-22
#========================================================================================

import os, app_code_configs, app_utils

#========================================================================================

# Global Variables - only global to this file
g_Date  = "na"
g_Owner = "na"

g_OutDir = "."

g_Tbls = []

#========================================================================================

def inits():
    global g_OutDir, g_Owner, g_Date, g_Tbls

    g_Date = app_code_configs.get_date()
    g_OutDir = app_code_configs.get_outdir()
    g_Owner = app_code_configs.get_owner()

    g_Tbls.extend(app_code_configs.get_tables())

#========================================================================================

def construct_header_content(tgtApp):
    global g_Owner, g_Date, g_Tbls

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    head = """\
/*=======================================================================================
** File Name:  %s_tbldefs.h
**
** Title:  Header File for %s Application's tables
**
** $Author:    %s
** $Revision: 1.1 $
** $Date:      %s
**
** Purpose:  This header file contains declarations and definitions of data structures
**           used in %s's tables.
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   %s | %s | Build #: Code Started
**
**=====================================================================================*/
    """ % (lcApp, ucApp, g_Owner, g_Date, ucApp, g_Date, g_Owner)

    tmpStr = ucApp + "_TBLDEFS_H"

    part1 = """
#ifndef _%s_
#define _%s_

/*
** Pragmas
*/

/*
** Include Files
*/
#include \"common_types.h\"
#include \"%s_mission_cfg.h\"

/*
** Local Defines
*/\n""" % (tmpStr, tmpStr, lcApp)

    part2 = ""
    if "iload" in g_Tbls:
        part2 = """\
#define %s_ILOAD_MAX_ENTRIES  1
#define %s_ILOAD_FILENAME     \"/cf/apps/%s_iloads.tbl\"
#define %s_ILOAD_TABLENAME    \"ILOAD_TBL\"\n\n""" % (ucApp, ucApp, lcApp, ucApp)

    part3 = ""
    if "cds" in g_Tbls:
        part3 = """\
#define %s_CDS_TABLENAME  \"%s_CdsTbl\"
    """ % (ucApp, lcApp)

    part4 = """
/*
** Local Structure Declarations
*/ """

    part5 = ""
    if "iload" in g_Tbls:
        part5 = """
/* Definition for Iload table entry */
typedef struct
{
    int32  iParam;

    /* TODO:  Add type declaration for ILoad parameters here.
    **
    ** Examples:
    **    int8/char            cParam;
    **    int8/char            cParams[16];
    **    uint8/unsigned char  ucParam;
    **    uint8/unsigned char  ucParams[16];
    **
    **    int16   sParam;
    **    int16   sParams[8];
    **    uint16  usParam;
    **    uint16  usParams[8];
    **
    **    int32   iParam;
    **    int32   iParams[5];
    **    uint32  uiParam;
    **    uint32  uiParams[5];
    **
    **    float  fParam;
    **    float  fParams[3];
    **
    **    double  dParam;
    **    double  dParams[3];
    */
} %s_ILoadTblEntry_t;
    """ % (ucApp)

    part6 = ""
    if "cds" in g_Tbls:
        part6 = """
/* Definition for Critical Data Storage (CDS) table entry */
typedef struct
{
    int32  iParam;

    /* TODO:  Add type declaration for CDS data here. */
} %s_CdsTbl_t;
    """ % (ucApp)

    part7 = """
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

#endif /* _%s_ */

/*=======================================================================================
** End of file %s_tbldefs.h
**=====================================================================================*/
    """ % (tmpStr, lcApp)

    content = head + part1 + part2 + part3 + part4 + part5 + part6 + part7

    return content

#========================================================================================

def construct_source_iload_content(tgtApp):
    global g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    content = """\
/*=======================================================================================
** File Name:  %s_iloads.c
**
** Title:  Default Iload Table for %s Application
**
** $Author:    %s
** $Revision: 1.1 $
** $Date:      %s
**
** Purpose:  This source file contains definition of table content for %s application's 
**           default ILoad table.
**
** Functions Defined:
**    None
**
** Limitations, Assumptions, External Events, and Notes:
**    1. One source file per CFS table!
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
#include \"cfe_tbl_filedef.h\"
#include \"%s_tbldefs.h\"

/*
** Local Defines
*/

/*
** Local Structure Declarations
*/
static CFE_TBL_FileDef_t CFE_TBL_FileDef =
{
    /* Content format: ObjName[64], TblName[38], Desc[32], TgtFileName[20], ObjSize 
    **    ObjName - variable name of ILoad table, e.g., %s_ILoadDefTbl[]
    **    TblName - app's table name, e.g., %s.ILOAD_TBL, where %s is the same app name
    **              used in cfe_es_startup.scr, and %s_defILoadTbl is the same table
    **              name passed in to CFE_TBL_Register()
    **    Desc - description of table in string format
    **    TgtFileName[20] - table file name, compiled as .tbl file extension
    **    ObjSize - size of the entire table
    */

    \"%s_ILoadDefTbl\", \"%s.ILOAD_TBL\", \"%s default ILoad table\",
    \"%s_iloads.tbl\", (sizeof(%s_ILoadTblEntry_t) * %s_ILOAD_MAX_ENTRIES)
};

/*
** External Global Variables
*/

/*
** Global Variables
*/

/* Default %s iLoad table data */
%s_ILoadTblEntry_t %s_ILoadDefTbl[%s_ILOAD_MAX_ENTRIES] =
{
    /* Entry 1 */
    {
        .iParam = 123

        /* TODO:  Add default values for ILoad parameters here.
        **
        ** Examples: (See example of type declarations in %s_tbldefs.h)
        **    .cParam   = 7,
        **    .cParams  = "Init Str Value",
        **    .ucParam  = 'A',
        **    .ucParams = "/dev/ttyUSB",
        **
        **    .sParam   = -16,
        **    .sParams  = {-3, -2, -1, 0, 1, 2, 3, 4},
        **    .usParam  = 16,
        **    .usParams = {1, 2, 3, 4, 5, 6, 7, 8},
        **
        **    .iParam   = -32,
        **    .iParams  = {-123, -234, 0, 123, 234},
        **    .uiParam  = -32,
        **    .uiParams = {123, 234, 345, 456, 678},
        **
        **    .fParam  = 32.1234,
        **    .fParams = {-12.34, 0.0, 12.34},
        **
        **    .dParam = 123.456789,
        **    .dParams = {-123.456789, 0.0, 123.456789}
        */
    }
};

/*
** Local Variables
*/

/*
** Function Prototypes
*/

/*
** Function Definitions
*/

/*=======================================================================================
** End of file %s_iloads.c
**=====================================================================================*/
    """ % (lcApp, ucApp, g_Owner, g_Date, ucApp, g_Date, g_Owner, lcApp, ucApp, ucApp, \
           ucApp, ucApp, ucApp, ucApp, ucApp, lcApp, ucApp, ucApp, ucApp, ucApp, ucApp, \
           ucApp, lcApp, lcApp)

    return content

#========================================================================================

def construct_iload_util_header_content(tgtApp):
    global g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    # Construct file description
    head = """\
/*=======================================================================================
** File Name:  %s_iload_utils.h
**
** Title:  ILoad Tables' Utility Header File for %s Application
**
** $Author:    %s
** $Revision: 1.1 $
** $Date:      %s
**
** Purpose:  To define %s's ILoad table-related utility functions
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   %s | %s | Build #: Code Started
**
**=====================================================================================*/
    """ % (lcApp, ucApp, g_Owner, g_Date, ucApp, g_Date, g_Owner)

    # Construct file content
    tmpStr = ucApp + "_ILOAD_UTILS_H"
    body = """
#ifndef _%s_
#define _%s_

/*
** Pragmas
*/

/*
** Include Files
*/
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

/*
** Local Variables
*/

/*
** Local Function Prototypes
*/
int32  %s_InitILoadTbl(void);
int32  %s_ValidateILoadTbl(%s_ILoadTblEntry_t*);
void   %s_ProcessNewILoadTbl(void);

#endif /* _%s_ */

/*=======================================================================================
** End of file %s_iload_utils.h
**=====================================================================================*/
    """ % (tmpStr, tmpStr, lcApp, ucApp, ucApp, ucApp, ucApp, tmpStr, lcApp)

    content = head + body

    return content

#========================================================================================

def construct_source_iload_util_head_content(tgtApp):
    global g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    content = """\
/*=======================================================================================
** File Name:  %s_iload_utils.c
**
** Title:  Iload Tables' Utilities for %s Application
**
** $Author:    %s
** $Revision: 1.1 $
** $Date:      %s
**
** Purpose:  This source file contains definitions of ILoad table-related utility
**           function for %s application.
**
** Functions Defined:
**    None
**
** Limitations, Assumptions, External Events, and Notes:
**    1. One source file per CFS table!
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
#include \"%s_iload_utils.h\"

/*
** Local Defines
*/

/*
** Local Structure Declarations
*/

/*
** External Global Variables
*/
extern %s_AppData_t  g_%s_AppData;

/*
** Global Variables
*/

/*
** Local Variables
*/

/*
** Function Prototypes
*/

/*
** Function Definitions
*/
    """ % (lcApp, ucApp, g_Owner, g_Date, ucApp, g_Date, g_Owner, lcApp, ucApp, ucApp)

    return content

#========================================================================================

def construct_source_iload_util_tail_content(tgtApp):
    content = """
/*=======================================================================================
** End of file %s_iload_utils.c
**=====================================================================================*/
    """ % (tgtApp.lower())

    return content

#========================================================================================

def construct_source_iload_util_init_content(tgtApp):
    global g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    content = """
/*=====================================================================================
** Name: %s_InitILoadTbl
**
** Purpose: To initialize the %s's ILoad tables
**
** Arguments:
**    None
**
** Returns:
**    int32 iStatus - Status of initialization
**
** Routines Called:
**    CFE_TBL_Register
**    CFE_TBL_Load
**    CFE_TBL_Manage
**    CFE_TBL_GetAddress
**    CFE_ES_WriteToSysLog
**    %s_ValidateILoadTbl
**    %s_ProcessNewILoadTbl
**
** Called By:
**    %s_InitApp
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
** Programmer(s):  GSFC, %s 
**
** History:  Date Written  %s
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 %s_InitILoadTbl()
{
    int32  iStatus=0;

    /* Register ILoad table */
    iStatus = CFE_TBL_Register(&g_%s_AppData.ILoadTblHdl,
                               %s_ILOAD_TABLENAME,
                               (sizeof(%s_ILoadTblEntry_t) * %s_ILOAD_MAX_ENTRIES),
                               CFE_TBL_OPT_DEFAULT,
                               %s_ValidateILoadTbl);
    if (iStatus != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog(\"%s - Failed to register ILoad table (0x%%08X)\\n\", iStatus);
        goto %s_InitILoadTbl_Exit_Tag;
    }

    /* Load ILoad table file */
    iStatus = CFE_TBL_Load(g_%s_AppData.ILoadTblHdl,
                           CFE_TBL_SRC_FILE,
                           %s_ILOAD_FILENAME);
    if (iStatus != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog(\"%s - Failed to load ILoad Table (0x%%08X)\\n\", iStatus);
        goto %s_InitILoadTbl_Exit_Tag;
    }

    /* Manage ILoad table */
    iStatus = CFE_TBL_Manage(g_%s_AppData.ILoadTblHdl);
    if (iStatus != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog(\"%s - Failed to manage ILoad table (0x%%08X)\\n\", iStatus);
        goto %s_InitILoadTbl_Exit_Tag;
    }

    /* Make sure ILoad table is accessible by getting referencing it */
    iStatus = CFE_TBL_GetAddress((void*)&g_%s_AppData.ILoadTblPtr,
                                 g_%s_AppData.ILoadTblHdl);
    if (iStatus != CFE_TBL_INFO_UPDATED)
    {
        CFE_ES_WriteToSysLog(\"%s - Failed to get ILoad table's address (0x%%08X)\\n\", iStatus);
        goto %s_InitILoadTbl_Exit_Tag;
    }

    /* Validate ILoad table */
    iStatus = %s_ValidateILoadTbl(g_%s_AppData.ILoadTblPtr);
    if (iStatus != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog(\"%s - Failed to validate ILoad table (0x%%08X)\\n\", iStatus);
        goto %s_InitILoadTbl_Exit_Tag;
    }

    /* Set new parameter values */
    %s_ProcessNewILoadTbl();

%s_InitILoadTbl_Exit_Tag:
    return (iStatus);
}
    """ % (ucApp, ucApp, ucApp, ucApp, ucApp, g_Owner, g_Date, ucApp, ucApp, ucApp, \
           ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, \
           ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp)

    return content

#========================================================================================

def construct_source_iload_util_validate_content(tgtApp):
    global g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    content = """
/*=====================================================================================
** Name: %s_ValidateILoadTbl
**
** Purpose: To validate the %s's ILoad tables
**
** Arguments:
**    %s_ILoadTblEntry_t*  iLoadTblPtr - pointer to the ILoad table
**
** Returns:
**    int32 iStatus - Status of table updates
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
** Programmer(s):  %s 
**
** History:  Date Written  %s
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 %s_ValidateILoadTbl(%s_ILoadTblEntry_t* iLoadTblPtr)
{
    int32  iStatus=0;

    if (iLoadTblPtr == NULL)
    {
        iStatus = -1;
        goto %s_ValidateILoadTbl_Exit_Tag;
    }

    /* TODO:  Add code to validate new data values here.
    **
    ** Examples:
    **    if (iLoadTblPtr->sParam <= 16)
    **    {
    **        CFE_ES_WriteToSysLog(\"%s - Invalid value for ILoad parameter sParam (%%d)\\n\",
    **                             iLoadTblPtr->sParam);
    */

%s_ValidateILoadTbl_Exit_Tag:
    return (iStatus);
}
    """ % (lcApp, ucApp, ucApp, g_Owner, g_Date, ucApp, ucApp, ucApp, ucApp, ucApp)

    return content

#========================================================================================

def construct_source_iload_util_process_content(tgtApp):
    global g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    content = """
/*=====================================================================================
** Name: %s_ProcessNewILoadTbl
**
** Purpose: To process %s's new ILoad tables and set ILoad parameters with new values
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
** Programmer(s):  %s 
**
** History:  Date Written  %s
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void %s_ProcessNewILoadTbl()
{
    /* TODO:  Add code to set new ILoad parameters with new values here.
    **
    ** Examples:
    **
    **    g_%s_AppData.latest_sParam = g_%s_AppData.ILoadTblPtr->sParam;
    **    g_%s_AppData.latest_fParam = g_%s.AppData.ILoadTblPtr->fParam;
    */
}
    """ % (ucApp, ucApp, g_Owner, g_Date, ucApp, ucApp, ucApp, ucApp, ucApp)

    return content

#========================================================================================

def construct_cds_util_header_content(tgtApp):
    global g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    content = """\
/*=======================================================================================
** File Name:  %s_cds_utils.h
**
** Title:  Critical Data Store (CDS) Tables' Utility Header File for %s Application
**
** $Author:    %s
** $Revision: 1.1 $
** $Date:      %s
**
** Purpose:  To define %s's CDS table-related utility functions
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   %s | %s | Build #: Code Started
**
**=====================================================================================*/

#ifndef _%s_CDS_UTILS_H_
#define _%s_CDS_UTILS_H_

/*
** Pragmas
*/

/*
** Include Files
*/
#include \"%s_tbldefs.h\"

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

/*
** Local Variables
*/

/*
** Local Function Prototypes
*/
int32  %s_InitCdsTbl(void);
void   %s_UpdateCdsTbl(void);
void   %s_SaveCdsTbl(void);

#endif /* _%s_CDS_UTILS_H_ */

/*=======================================================================================
** End of file %s_cds_utils.h
**=====================================================================================*/
    """ % (lcApp, ucApp, g_Owner, g_Date, ucApp, g_Date, g_Owner, ucApp, ucApp, lcApp, \
           ucApp, ucApp, ucApp, ucApp, lcApp)

    return content

#========================================================================================

def construct_source_cds_util_head_content(tgtApp):
    global g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    content = """\
/*=======================================================================================
** File Name:  %s_cds_utils.c
**
** Title:  Critical Data Store (CDS) Tables' Utilities for %s Application
**
** $Author:    %s
** $Revision: 1.1 $
** $Date:      %s
**
** Purpose:  This source file contains definitions of CDS table-related utility
**           function for %s application.
**
** Functions Defined:
**    None
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
#include "%s_app.h"
#include "%s_cds_utils.h"

/*
** Local Defines
*/

/*
** Local Structure Declarations
*/

/*
** External Global Variables
*/
extern %s_AppData_t  g_%s_AppData;

/*
** Global Variables
*/

/*
** Local Variables
*/

/*
** Function Prototypes
*/

/*
** Function Definitions
*/
    """ % (lcApp, ucApp, g_Owner, g_Date, ucApp, g_Date, g_Owner, lcApp, lcApp, ucApp, ucApp)

    return content

#========================================================================================

def construct_source_cds_util_init_content(tgtApp):
    global g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    content = """
/*=====================================================================================
** Name: %s_InitCdsTbl
**
** Purpose: To initialize the %s's Cds tables
**
** Arguments:
**    None
**
** Returns:
**    int32 iStatus - Status of initialization
**
** Routines Called:
**    CFE_ES_GetResetType
**    CFE_ES_RegisterCDS
**    CFE_ES_CopyToCDS
**    CFE_ES_WriteToSysLog
**    CFE_ES_RestoreFromCDS
**
** Called By:
**    %s_InitApp
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
** Programmer(s):  %s 
**
** History:  Date Written  %s
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 %s_InitCdsTbl()
{
    int32  iStatus=0;
    int32  iResetType=0;
    uint32 uiResetSubType=0;

    /* Only clear CDS table when it's a PowerOn reset, and not a Processor reset */
    iResetType = CFE_ES_GetResetType(&uiResetSubType);
    if (iResetType == CFE_ES_POWERON_RESET)
    {
        memset((void*)&g_%s_AppData.CdsTbl, 0x00, sizeof(%s_CdsTbl_t));
    }

    /* Register and manage CDS table */
    iStatus = CFE_ES_RegisterCDS(&g_%s_AppData.CdsTblHdl, sizeof(%s_CdsTbl_t),
                                 %s_CDS_TABLENAME);
    if (iStatus == CFE_SUCCESS)
    {
        /* Setup initial content of CDS table */
        iStatus = CFE_ES_CopyToCDS(g_%s_AppData.CdsTblHdl, &g_%s_AppData.CdsTbl);
        if (iStatus == CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog(\"%s - Successfully setup CDS\\n\");
        }
        else
        {
            CFE_ES_WriteToSysLog(\"%s - Failed to setup CDS\\n\");
        }
    }
    else if (iStatus == CFE_ES_CDS_ALREADY_EXISTS)
    {
        /* If one already exists, get a copy of its current content */
        memset((void*)&g_%s_AppData.CdsTbl, 0x00, sizeof(%s_CdsTbl_t));
        iStatus = CFE_ES_RestoreFromCDS(&g_%s_AppData.CdsTbl, g_%s_AppData.CdsTblHdl);
        if (iStatus == CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog(\"%s - Successfully restored data from CDS\\n\");
        }
        else
        {
            CFE_ES_WriteToSysLog(\"%s - Failed to restore data from CDS\\n\");
            memset((void*)&g_%s_AppData.CdsTbl, 0x00, sizeof(%s_CdsTbl_t));
        }
    }
    else
    {
        CFE_ES_WriteToSysLog(\"%s - Failed to create CDS (%%d)\\n\", iStatus);
    }

    return (iStatus);
}
    """ % (ucApp, ucApp, ucApp, g_Owner, g_Date, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, \
           ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, \
           ucApp, ucApp)

    return content

#========================================================================================

def construct_source_cds_util_update_content(tgtApp):
    global g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    content = """
/*=====================================================================================
** Name: %s_UpdateCdsTbl
**
** Purpose: To update the %s's CDS table with new values
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
**
** Programmer(s):  %s
**
** History:  Date Written  %s 
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void %s_UpdateCdsTbl()
{
    /* TODO:  Add code to update values in CDS table here */
}
    """ % (ucApp, ucApp, g_Owner, g_Date, ucApp)

    return content

#========================================================================================

def construct_source_cds_util_save_content(tgtApp):
    global g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    content = """
/*=====================================================================================
** Name: %s_SaveCdsTbl
**
** Purpose: To save the current CDS table
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**    CFE_ES_CopyToCDS
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
** Programmer(s):  %s
**
** History:  Date Written  %s
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void %s_SaveCdsTbl()
{
    CFE_ES_CopyToCDS(g_%s_AppData.CdsTblHdl, &g_%s_AppData.CdsTbl);
}
    """ % (ucApp, g_Owner, g_Date, ucApp, ucApp, ucApp)

    return content

#========================================================================================

def construct_source_cds_util_tail_content(tgtApp):
    content = """
/*=======================================================================================
** End of file %s_cds_utils.c
**=====================================================================================*/
    """ % (tgtApp.lower())

    return content

#========================================================================================

def generate_header(tgtApp):
    global g_OutDir, g_Tbls

    # Construct file path
    name = tgtApp.lower() + "_tbldefs.h"
    filePath = os.path.join(g_OutDir, tgtApp.lower(), "fsw", "tables", name)
    fileExist_p = os.path.exists(filePath)

    if "none" in g_Tbls:
        # Remove file
        app_utils.remove_file(filePath)
    elif "iload" in g_Tbls or "cds" in g_Tbls:
        # Open file
        fileHdl = app_utils.open_file_for_writing(filePath, False)
        # Write to file
        fileHdl.write(construct_header_content(tgtApp))
        # Close file
        app_utils.close_file_from_writing(fileHdl, filePath, fileExist_p)
    else:
        pass

#========================================================================================

def generate_default_iload_source(tgtApp):
    global g_OutDir, g_Tbls

    # Construct file path
    name = tgtApp.lower() + "_iloads.c"
    filePath = os.path.join(g_OutDir, tgtApp.lower(), "fsw", "tables", name)
    fileExist_p = os.path.exists(filePath)

    if "none" in g_Tbls or "iload" not in g_Tbls:
        # Remove file
        app_utils.remove_file(filePath)
    elif "iload" in g_Tbls:
        # Open file
        fileHdl = app_utils.open_file_for_writing(filePath, False)
        # Write to file
        fileHdl.write(construct_source_iload_content(tgtApp))
        # Close file
        app_utils.close_file_from_writing(fileHdl, filePath, fileExist_p)
    else:
        pass

#========================================================================================

def generate_iload_utils_header(tgtApp):
    global g_OutDir, g_Tbls

    # Construct file path
    name = tgtApp.lower() + "_iload_utils.h"
    filePath = os.path.join(g_OutDir, tgtApp.lower(), "fsw", "tables", name)
    fileExist_p = os.path.exists(filePath)

    if "none" in g_Tbls or "iload" not in g_Tbls:
        # Remove file
        app_utils.remove_file(filePath)
    elif "iload" in g_Tbls:
        # Open file
        fileHdl = app_utils.open_file_for_writing(filePath, False)
        # Write to file
        fileHdl.write(construct_iload_util_header_content(tgtApp))
        # Close file
        app_utils.close_file_from_writing(fileHdl, filePath, fileExist_p)
    else:
        pass

#========================================================================================

def generate_iload_utils_source(tgtApp):
    global g_OutDir, g_Tbls

    # Construct file path
    name = tgtApp.lower() + "_iload_utils.c"
    filePath = os.path.join(g_OutDir, tgtApp.lower(), "fsw", "tables", name)
    fileExist_p = os.path.exists(filePath)

    if "none" in g_Tbls or "iload" not in g_Tbls:
        # Remove file
        app_utils.remove_file(filePath)
    elif "iload" in g_Tbls:
        # Open file
        fileHdl = app_utils.open_file_for_writing(filePath, False)
        # Write to file
        fileHdl.write(construct_source_iload_util_head_content(tgtApp))
        fileHdl.write(construct_source_iload_util_init_content(tgtApp))
        fileHdl.write(construct_source_iload_util_validate_content(tgtApp))
        fileHdl.write(construct_source_iload_util_process_content(tgtApp))
        fileHdl.write(construct_source_iload_util_tail_content(tgtApp))
        # Close file
        app_utils.close_file_from_writing(fileHdl, filePath, fileExist_p)
    else:
        pass

#========================================================================================

def generate_cds_utils_header(tgtApp):
    global g_OutDir, g_Tbls

    # Construct file path
    name = tgtApp.lower() + "_cds_utils.h"
    filePath = os.path.join(g_OutDir, tgtApp.lower(), "fsw", "tables", name)
    fileExist_p = os.path.exists(filePath)

    if "none" in g_Tbls or "cds" not in g_Tbls:
        # Remove file
        app_utils.remove_file(filePath)
    elif "cds" in g_Tbls:
        # Open file
        fileHdl = app_utils.open_file_for_writing(filePath, False)
        # Write to file
        fileHdl.write(construct_cds_util_header_content(tgtApp))
        # Close file
        app_utils.close_file_from_writing(fileHdl, filePath, fileExist_p)
    else:
        pass

#========================================================================================

def generate_cds_utils_source(tgtApp):
    global g_OutDir, g_Tbls

    # Construct file path
    name = tgtApp.lower() + "_cds_utils.c"
    filePath = os.path.join(g_OutDir, tgtApp.lower(), "fsw", "tables", name)
    fileExist_p = os.path.exists(filePath)

    if "none" in g_Tbls or "cds" not in g_Tbls:
        # Remove file
        app_utils.remove_file(filePath)
    elif "cds" in g_Tbls:
        # Open file
        fileHdl = app_utils.open_file_for_writing(filePath, False)
        # Write to file
        fileHdl.write(construct_source_cds_util_head_content(tgtApp))
        fileHdl.write(construct_source_cds_util_init_content(tgtApp))
        fileHdl.write(construct_source_cds_util_update_content(tgtApp))
        fileHdl.write(construct_source_cds_util_save_content(tgtApp))
        fileHdl.write(construct_source_cds_util_tail_content(tgtApp))
        # Close file
        app_utils.close_file_from_writing(fileHdl, filePath, fileExist_p)
    else:
        pass

#========================================================================================
# End of app_code_tables.py
#========================================================================================

