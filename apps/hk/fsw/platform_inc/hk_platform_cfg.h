/************************************************************************
** File:
**   $Id: hk_platform_cfg.h 1.8 2015/03/04 14:58:29EST sstrege Exp  $
**
**  Copyright � 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** Purpose: 
**  The CFS Housekeeping (HK) Application platform configuration header file
**
** Notes:
**
** $Log: hk_platform_cfg.h  $
** Revision 1.8 2015/03/04 14:58:29EST sstrege 
** Added copyright information
** Revision 1.7 2012/08/15 18:32:39EDT aschoeni 
** Added ability to discard incomplete combo packets
** Revision 1.6 2011/06/23 12:00:02EDT jmdagost 
** Moved HK_MISSION_REV from version header to here.
** Revision 1.5 2010/05/06 15:32:55EDT jmdagost 
** Changed location of hk_cpy_tbl.tbl from /cf/ to /cf/apps/
** Revision 1.4 2009/12/03 16:32:17EST jmdagost 
** Expanded comment on mempool size definition, corrected copy table filename.
** Revision 1.3 2009/04/18 12:55:14EDT dkobe 
** Updates to correct doxygen comments
** Revision 1.2 2008/05/07 09:55:22EDT rjmcgraw 
** DCR1647:3 Romoved the Hk from copy table name and runtime table name
** Revision 1.1 2008/04/09 16:39:33EDT rjmcgraw 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hk/fsw/platform_inc/project.pj
**
*************************************************************************/
#ifndef _hk_platform_cfg_h_
#define _hk_platform_cfg_h_



/*************************************************************************
** Macro definitions
**************************************************************************/

/**
**  \hkcfg Application Pipe Depth 
**
**  \par Description:
**       Dictates the pipe depth of the hk command pipe.
**
**  \par Limits
**		 The minimum size of this paramater is 1
**       The maximum size dictated by cFE platform configuration 
**		 parameter  CFE_SB_MAX_PIPE_DEPTH
*/
#define HK_PIPE_DEPTH       40


/**
**  \hkcfg Discard Incomplete Combo Packets 
**
**  \par Description:
**       Dictates whether combo packets that have not had all data contents
**       updated since last requested will be discarded (YES = 1) or sent 
**       anyway (NO = 0).
**
**  \par Limits
**       This parameter can be set to 0 or 1 only.
*/
#define HK_DISCARD_INCOMPLETE_COMBO          0


/**
**  \hkcfg Maximum Number of HK Copy Table Entries 
**
**  \par Description:
**       Dictates the number of elements in the hk copy table.
**
**  \par Limits
**       The maximum size of this paramater is 8192
*/
#define HK_COPY_TABLE_ENTRIES          128


/**
**  \hkcfg Number of bytes in the HK Memory Pool 
**
**  \par Description:
**       The HK memory pool contains the memory needed for the output packets.
**       The output packets are dynamically allocated from this pool when the
**       HK copy table is initially processed or loaded with new data.
**
**  \par Limits
**       The Housekeeping app does not place a limit on this parameter, but there is
**       an overhead cost in the memory pool.  The value must be larger than what is
**       needed.
*/
#define HK_NUM_BYTES_IN_MEM_POOL        (6 * 1024)


/**
**  \hkcfg Name of the HK Copy Table 
**
**  \par Description:
**       This parameter defines the name of the HK Copy Table. 
**
**  \par Limits
**       The Housekeeping app does not place a limit on this parameter
*/
#define HK_COPY_TABLE_NAME      "CopyTable"


/**
**  \hkcfg Name of the HK Run-time Table 
**
**  \par Description:
**       This parameter defines the name of the HK Run-time Table. 
**
**  \par Limits
**       The Housekeeping app does not place a limit on this parameter
*/
#define HK_RUNTIME_TABLE_NAME       "RuntimeTable"


/**
**  \hkcfg HK Copy Table Filename
**
**  \par Description:
**       The value of this constant defines the filename of the HK Copy Table
**
**  \par Limits
**       The length of each string, including the NULL terminator cannot exceed 
**       the #OS_MAX_PATH_LEN value.
*/

#define HK_COPY_TABLE_FILENAME  "/cf/hk_cpy_tbl.tbl"

/** \hkcfg Mission specific version number for HK application
**  
**  \par Description:
**       An application version number consists of four parts:
**       major version number, minor version number, revision
**       number and mission specific revision number. The mission
**       specific revision number is defined here and the other
**       parts are defined in "hk_version.h".
**
**  \par Limits:
**       Must be defined as a numeric value that is greater than
**       or equal to zero.
*/
#define HK_MISSION_REV            0


#endif /* _hk_platform_cfg_h_ */

/************************/
/*  End of File Comment */
/************************/
