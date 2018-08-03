/*
** $Id: fm_app.h 1.15 2015/02/28 17:50:48EST sstrege Exp  $
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
** Title: CFS File Manager (FM) Application Header File
**
** Purpose: Unit specification for the CFS File Manager Application.
**
** Author: Susanne L. Strege, Code 582 NASA GSFC
**
** Notes:
**
** References:
**    Flight Software Branch C Coding Standard Version 1.0a
**
** $Log: fm_app.h  $
** Revision 1.15 2015/02/28 17:50:48EST sstrege 
** Added copyright information
** Revision 1.14 2009/11/09 16:52:29EST lwalling 
** Cleanup and expand function prototype comments, move struct defs to fm_msg.h
** Revision 1.13 2009/10/30 14:02:29EDT lwalling 
** Remove trailing white space from all lines
** Revision 1.12 2009/10/30 10:41:40EDT lwalling
** Modify field names in child task queue structure
** Revision 1.11 2009/10/29 11:42:26EDT lwalling
** Make common structure for open files list and open file telemetry packet, change open file to open files
** Revision 1.10 2009/10/27 17:32:30EDT lwalling
** Add a child task command warning counter, Make file buffer cfg defs common for all child cmd handlers
** Revision 1.9 2009/10/26 16:47:36EDT lwalling
** Add GetFileInfo args to child queue, changes to global structures, add global packets
** Revision 1.8 2009/10/26 11:31:03EDT lwalling
** Remove Close File command from FM application
** Revision 1.7 2009/10/23 14:42:35EDT lwalling
** Define child task argument structure, create child task arg queue, create child task status vars
** Revision 1.6 2009/10/16 15:42:19EDT lwalling
** Add warning counter, global dir list packet, update structure names
** Revision 1.5 2009/10/09 17:23:49EDT lwalling
** Create command to generate file system free space packet, replace device table with free space table
** Revision 1.4 2009/10/07 15:58:44EDT lwalling
** Removed unused variable DeviceTablePresent from global data structure
** Revision 1.3 2009/09/28 14:15:31EDT lwalling
** Create common filename verification functions
** Revision 1.2 2008/06/20 16:21:22EDT slstrege
** Member moved from fsw/src/fm_app.h in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/cfs_fm.pj to fm_app.h in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/fsw/src/project.pj.
** Revision 1.1 2008/06/20 15:21:22ACT slstrege
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/cfs_fm.pj
*/

#ifndef _fm_app_h_
#define _fm_app_h_

#include "cfe.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM application global function prototypes                       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \brief Application entry point and main process loop
**
**  \par Description
**
**       Register FM as a CFE application.
**       Invoke FM application initialization function.
**       Enter FM main process loop.
**         Pend (forever) on next Software Bus command packet.
**         Process received Software Bus command packet.
**         Repeat main process loop.
**       Allow CFE to terminate the FM application.
**
**  \par Assumptions, External Events, and Notes: None
**
**  \sa #FM_AppInit, #CFE_ES_RunLoop, #CFE_SB_RcvMsg, #FM_ProcessPkt
**/
void FM_AppMain(void);


/**
**  \brief FM Application Initialization Function
**
**  \par Description
**
**       Initialize FM global data structure.
**       Register FM application for CFE Event Services.
**       Create Software Bus input pipe.
**       Subscribe to FM housekeeping request command packet.
**       Subscribe to FM ground command packet.
**       Invoke FM table initialization function.
**       Invoke FM child task initialization function.
**
**  \par Assumptions, External Events, and Notes: None
**
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS
**  \endcode
**  \retstmt Return codes from #CFE_EVS_Register, #CFE_SB_CreatePipe, #CFE_SB_Subscribe
**  \endcode
**  \endreturns
**
**  \sa #CFE_EVS_Register, #CFE_SB_CreatePipe, #CFE_SB_Subscribe
**/
int32 FM_AppInit(void);


/**
**  \brief Process Input Command Packets
**
**  \par Description
**
**       Branch to appropriate input packet handler: HK request or FM commands.
**
**  \par Assumptions, External Events, and Notes: None
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \sa #FM_ReportHK, #FM_ProcessCmd
**/
void FM_ProcessPkt(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Process FM Ground Commands
**
**  \par Description
**
**       Branch to the command specific handlers for FM ground commands.
**
**  \par Assumptions, External Events, and Notes: None
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \sa #FM_Noop, #FM_Reset, #FM_Copy, #FM_Move, #FM_Rename, #FM_Delete,
**      #FM_DeleteAll, #FM_Decompress, #FM_Concat, #FM_GetFileInfo,
**      #FM_GetOpenFiles, #FM_CreateDir, #FM_DeleteDir, #FM_GetDirFile,
**      #FM_GetDirPkt, #FM_GetFreeSpace
**/
void FM_ProcessCmd(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Housekeeping Request Command Handler
**
**  \par Description
**
**       Allow CFE Table Services the opportunity to manage the File System
**       Free Space Table.  This provides a mechanism to receive table updates.
**       
**       Populate the FM application Housekeeping Telemetry packet.  Timestamp
**       the packet and send it to ground via the Software Bus.
**
**  \par Assumptions, External Events, and Notes: None
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \sa #FM_HousekeepingCmd_t, #FM_HousekeepingPkt_t
**/
void FM_ReportHK(CFE_SB_MsgPtr_t MessagePtr);


#endif /* _fm_app_h_ */

/************************/
/*  End of File Comment */
/************************/
