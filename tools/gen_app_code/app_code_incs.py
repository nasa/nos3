#========================================================================================
# File:    app_code_incs.py
# Author:  Tam Ngo/JSC
# Date:    2012-02-22
#
# Modification History:
#   Date | Author | Description
#   ---------------------------
#   10/7/15 | Susanne Strege | Added message ids, and performance header definitions and 
#                              generation.  Updated mission and platform  configuration 
#                              header definitions.
#
#   03/31/16| Michael Rosburg | Updated message ids and removed obsolete header comments.
#   05/25/16| Michael Rosburg | Moved pipe depth macros into the _platform_cfg.h
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

    g_Mission = app_code_configs.get_mission()
    g_Date = app_code_configs.get_date()
    g_OutDir = app_code_configs.get_outdir()
    g_Owner = app_code_configs.get_owner()

    g_Tbls.extend(app_code_configs.get_tables())

#========================================================================================

def construct_platform_cfg_content(tgtApp):
    global g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    head = """\
/*=======================================================================================
** File Name:  %s_platform_cfg.h
**
** Title:  Platform Configuration Header File for %s Application
**
** $Author:    %s
** $Revision: 1.1 $
** $Date:      %s
**
** Purpose:  This header file contains declartions and definitions of all %s's 
**           platform-specific configurations.
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   %s | %s | Build #: Code Started
**
**=====================================================================================*/
    """ % (lcApp, ucApp, g_Owner, g_Date, ucApp, g_Date, g_Owner)

    tmpStr = ucApp + "_PLATFORM_CFG_H"
    body = """
#ifndef _%s_
#define _%s_

/*
** %s Platform Configuration Parameter Definitions
*/
#define %s_SCH_PIPE_DEPTH  2
#define %s_CMD_PIPE_DEPTH  10
#define %s_TLM_PIPE_DEPTH  10


/* TODO:  Add more platform configuration parameter definitions here, if necessary. */

#endif /* _%s_ */

/*=======================================================================================
** End of file %s_platform_cfg.h
**=====================================================================================*/
    """ % (tmpStr, tmpStr, lcApp, ucApp, ucApp, ucApp, tmpStr, lcApp)

    content = head + body

    return content

#========================================================================================

def construct_perfids_content(tgtApp):
    global g_Owner, g_Date, g_Mission, g_Tbls

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    head = """\
/*=======================================================================================
** File Name:  %s_perfids.h
**
** Title:  Performance ID Header File for %s Application
**
** $Author:    %s
** $Revision: 1.1 $
** $Date:      %s
**
** Purpose:  This header file contains declartions and definitions of all %s's 
**           Performance IDs.
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   %s | %s | Build #: Code Started
**
**=====================================================================================*/
    """ % (lcApp, ucApp, g_Owner, g_Date, ucApp, g_Date, g_Owner)

    tmpStr = ucApp + "_PERFIDS_H"
    ucMission = g_Mission.upper()

    part1 = """
#ifndef _%s_
#define _%s_

#define %s_MAIN_TASK_PERF_ID            50

    """ % (tmpStr, tmpStr, ucApp)

    if "iload" in g_Tbls or "cds" in g_Tbls:
        part2 = """#include \"%s_tbldefs.h\"\n""" % (lcApp)
    else:
        part2 = ""

    part3 = """


#endif /* _%s_ */

/*=======================================================================================
** End of file %s_perfids.h
**=====================================================================================*/
    """ % (tmpStr, lcApp)

    content = head + part1 + part2 + part3

    return content

#========================================================================================

def construct_msgids_content(tgtApp):
    global g_Owner, g_Date, g_Mission, g_Tbls

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    head = """\
/*=======================================================================================
** File Name:  %s_msgids.h
**
** Title:  Message ID Header File for %s Application
**
** $Author:    %s
** $Revision: 1.1 $
** $Date:      %s
**
** Purpose:  This header file contains declartions and definitions of all %s's 
**           Message IDs.
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   %s | %s | Build #: Code Started
**
**=====================================================================================*/
    """ % (lcApp, ucApp, g_Owner, g_Date, ucApp, g_Date, g_Owner)

    tmpStr = ucApp + "_MSGIDS_H"
    ucMission = g_Mission.upper()

    part1 = """
#ifndef _%s_
#define _%s_

/***** TODO:  These Message ID values are default and may need to be changed by the developer  *****/
#define %s_CMD_MID            	0x18C0
#define %s_SEND_HK_MID        	0x18C1
#define %s_WAKEUP_MID        	0x18D0
#define %s_OUT_DATA_MID        	0x18D1

#define %s_HK_TLM_MID		0x08BB

    """ % (tmpStr, tmpStr, ucApp, ucApp, ucApp, ucApp, ucApp)

    if "iload" in g_Tbls or "cds" in g_Tbls:
        part2 = """#include \"%s_tbldefs.h\"\n""" % (lcApp)
    else:
        part2 = ""

    part3 = """


#endif /* _%s_ */

/*=======================================================================================
** End of file %s_msgids.h
**=====================================================================================*/
    """ % (tmpStr, lcApp)

    content = head + part1 + part2 + part3

    return content

#========================================================================================

def construct_mission_cfg_content(tgtApp):
    global g_Owner, g_Date, g_Mission, g_Tbls

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    head = """\
/*=======================================================================================
** File Name:  %s_mission_cfg.h
**
** Title:  Mission Configuration Header File for %s Application
**
** $Author:    %s
** $Revision: 1.1 $
** $Date:      %s
**
** Purpose:  This header file contains declartions and definitions of all %s's 
**           mission-specific configurations.
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   %s | %s | Build #: Code Started
**
**=====================================================================================*/
    """ % (lcApp, ucApp, g_Owner, g_Date, ucApp, g_Date, g_Owner)

    tmpStr = ucApp + "_MISSION_CFG_H"
    ucMission = g_Mission.upper()

    part1 = """
#ifndef _%s_
#define _%s_

/*
** Include Files
*/
#include \"cfe.h\"

""" % (tmpStr, tmpStr)

    if "iload" in g_Tbls or "cds" in g_Tbls:
        part2 = """#include \"%s_tbldefs.h\"\n""" % (lcApp)
    else:
        part2 = ""

    part3 = """

/*
** %s Mission Configuration Parameter Definitions
*/

/* TODO:  Add mission configuration parameter definitions here, if necessary. */

#endif /* _%s_ */

/*=======================================================================================
** End of file %s_mission_cfg.h
**=====================================================================================*/
    """ % (ucApp, tmpStr, lcApp)

    content = head + part1 + part2 + part3

    return content

#========================================================================================

def construct_ids_content(tgtApp):
    global g_Owner, g_Date, g_Mission

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    head = """\
/*=======================================================================================
** File Name:  %s_private_ids.h
**
** Title:  ID Header File for %s Application
**
** $Author:    %s
** $Revision: 1.1 $
** $Date:      %s
**
** Purpose:  This header file contains declarations and definitions of %s's private IDs.
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   %s | %s | Build #: Code Started
**
**=====================================================================================*/
    """ % (lcApp, ucApp, g_Owner, g_Date, ucApp, g_Date, g_Owner)

    tmpStr = ucApp + "_PRIVATE_IDS_H"
    ucMission = g_Mission.upper()

    body = """
#ifndef _%s_
#define _%s_

/*
** Pragmas
*/

/*
** Include Files
*/

/*
** Local Defines
*/

/* Event IDs */
#define %s_RESERVED_EID  0

#define %s_INF_EID        1
#define %s_INIT_INF_EID   2
#define %s_ILOAD_INF_EID  3
#define %s_CDS_INF_EID    4
#define %s_CMD_INF_EID    5

#define %s_ERR_EID         51
#define %s_INIT_ERR_EID    52
#define %s_ILOAD_ERR_EID   53
#define %s_CDS_ERR_EID     54
#define %s_CMD_ERR_EID     55
#define %s_PIPE_ERR_EID    56
#define %s_MSGID_ERR_EID   57
#define %s_MSGLEN_ERR_EID  58

#define %s_EVT_CNT  14

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

#endif /* _%s_ */

/*=======================================================================================
** End of file %s_private_ids.h
**=====================================================================================*/
    """ % (tmpStr, tmpStr, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, \
           ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, ucApp, \
           tmpStr, lcApp)

    content = head + body

    return content

#========================================================================================

def construct_private_types_content(tgtApp):
    global g_Mission, g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    head = """\
/*=======================================================================================
** File Name:  %s_private_types.h
**
** Title:  Type Header File for %s Application
**
** $Author:    %s
** $Revision: 1.1 $
** $Date:      %s
**
** Purpose:  This header file contains declarations and definitions of all %s's private
**           data structures and data types.
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   %s | %s | Build #: Code Started
**
**=====================================================================================*/
    """ % (lcApp, ucApp, g_Owner, g_Date, ucApp, g_Date, g_Owner)

    tmpStr = ucApp + "_PRIVATE_TYPES_H"
    body = """
#ifndef _%s_
#define _%s_

/*
** Pragmas
*/

/*
** Include Files
*/
#include \"cfe.h\"

/*
** Local Defines
*/

/*
** Local Structure Declarations
*/

typedef struct
{
    uint8  ucCmdHeader[CFE_SB_CMD_HDR_SIZE];
} %s_NoArgCmd_t;


typedef struct
{
    uint32  counter;

    /* TODO:  Add input data to this application here, such as raw data read from I/O
    **        devices or data subscribed from other apps' output data.
    */

} %s_InData_t;

typedef struct
{
    uint8   ucTlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint32  uiCounter;
} %s_OutData_t;

/* TODO:  Add more private structure definitions here, if necessary. */

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
** End of file %s_private_types.h
**=====================================================================================*/
    """ % (tmpStr, tmpStr, ucApp, ucApp, ucApp, tmpStr, lcApp)

    content = head + body

    return content

#========================================================================================

def construct_global_types_content(tgtApp):
    global g_Mission, g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    head = """\
/*=======================================================================================
** File Name:  %s_%s_types.h
**
** Title:  Command/Telemetry Data Structure Header File for %s application
**
** $Author:    %s
** $Revision: 1.1 $
** $Date:      %s
**
** Purpose:  This header file contains definitions of command & telemetry data structures
**           for %s applications built for %s mission. 
**
**=====================================================================================*/
    """ % (g_Mission.upper(), lcApp, ucApp, g_Owner, g_Date, ucApp, g_Mission.upper())

    tmpStr = g_Mission.upper() + "_" + ucApp + "_TYPES_H"
    body = """
#ifndef _%s_
#define _%s_

/*
** Pragmas
*/

/*
** Include Files
*/
#include \"cfe.h\"

/*
** Local Defines
*/

/*
** Local Structure Declarations
*/
typedef struct
{
    uint8  ucCmdHeader[CFE_SB_CMD_HDR_SIZE];
} %s_NoArgCmd_t;

typedef struct
{
    uint8   ucTlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint16  usCmdCnt;
    uint16  usCmdErrCnt;
} %s_HkTlm_t;

typedef struct
{
    uint8   ucTlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint32  uiCounter;
} %s_OutData_t;

#endif /* _%s_ */

/*=======================================================================================
** End of file %s_%s_types.h
**=====================================================================================*/
    """ % (tmpStr, tmpStr, ucApp, ucApp, ucApp, tmpStr, g_Mission.upper(), lcApp)

    content = head + body

    return content

#========================================================================================

def generate_platform_cfg(tgtApp):
    global g_OutDir

    # Construct file path
    name = tgtApp.lower() + "_platform_cfg.h"
    filePath = os.path.join(g_OutDir, tgtApp.lower(), "fsw", "platform_inc", name)
    fileExist_p = os.path.exists(filePath)
    # Open file
    fileHdl = app_utils.open_file_for_writing(filePath, False)

    # Write to file
    fileHdl.write(construct_platform_cfg_content(tgtApp))

    # Close file
    app_utils.close_file_from_writing(fileHdl, filePath, fileExist_p)

#========================================================================================

def generate_msgids(tgtApp):
    global g_OutDir

    # Construct file path
    name = tgtApp.lower() + "_msgids.h"
    filePath = os.path.join(g_OutDir, tgtApp.lower(), "fsw", "platform_inc", name)
    fileExist_p = os.path.exists(filePath)
    # Open file
    fileHdl = app_utils.open_file_for_writing(filePath, False)

    # Write to file
    fileHdl.write(construct_msgids_content(tgtApp))

    # Close file
    app_utils.close_file_from_writing(fileHdl, filePath, fileExist_p)

#========================================================================================

def generate_perfids(tgtApp):
    global g_OutDir

    # Construct file path
    name = tgtApp.lower() + "_perfids.h"
    filePath = os.path.join(g_OutDir, tgtApp.lower(), "fsw", "mission_inc", name)
    fileExist_p = os.path.exists(filePath)
    # Open file
    fileHdl = app_utils.open_file_for_writing(filePath, False)

    # Write to file
    fileHdl.write(construct_perfids_content(tgtApp))

    # Close file
    app_utils.close_file_from_writing(fileHdl, filePath, fileExist_p)

#========================================================================================

def generate_mission_cfg(tgtApp):
    global g_OutDir

    # Construct file path
    name = tgtApp.lower() + "_mission_cfg.h"
    filePath = os.path.join(g_OutDir, tgtApp.lower(), "fsw", "mission_inc", name)
    fileExist_p = os.path.exists(filePath)
    # Open file
    fileHdl = app_utils.open_file_for_writing(filePath, False)

    # Write to file
    fileHdl.write(construct_mission_cfg_content(tgtApp))

    # Close file
    app_utils.close_file_from_writing(fileHdl, filePath, fileExist_p)

#========================================================================================

def generate_ids(tgtApp):
    global g_OutDir

    # Construct file path
    name = tgtApp.lower() + "_private_ids.h"
    filePath = os.path.join(g_OutDir, tgtApp.lower(), "fsw", "src", name)
    fileExist_p = os.path.exists(filePath)
    # Open file
    fileHdl = app_utils.open_file_for_writing(filePath, False)

    # Write to file
    fileHdl.write(construct_ids_content(tgtApp))

    # Close file
    app_utils.close_file_from_writing(fileHdl, filePath, fileExist_p)

#========================================================================================

def generate_private_types(tgtApp):
    global g_OutDir

    # Construct file path
    name = tgtApp.lower() + "_private_types.h"
    filePath = os.path.join(g_OutDir, tgtApp.lower(), "fsw", "src", name)
    fileExist_p = os.path.exists(filePath)
    # Open file
    fileHdl = app_utils.open_file_for_writing(filePath, False)

    # Write to file
    fileHdl.write(construct_private_types_content(tgtApp))

    # Close file
    app_utils.close_file_from_writing(fileHdl, filePath, fileExist_p)

#========================================================================================

def generate_global_types(tgtApp):
    global g_OutDir, g_Mission

    # Construct file path
    name = g_Mission.upper() + "_" + tgtApp.lower() + "_types.h"
    filePath = os.path.join(g_OutDir, tgtApp.lower(), "fsw", name)
    fileExist_p = os.path.exists(filePath)
    # Open file
    fileHdl = app_utils.open_file_for_writing(filePath, False)

    # Write to file
    fileHdl.write(construct_global_types_content(tgtApp))

    # Close file
    app_utils.close_file_from_writing(fileHdl, filePath, fileExist_p)

#========================================================================================
# End of app_code_incs.py
#========================================================================================

