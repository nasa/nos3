/************************************************************************
** File:
**   $Id: cf_cmds.h 1.6 2015/03/06 14:49:55EST sstrege Exp  $
**
**   Copyright © 2007-2014 United States Government as represented by the 
**   Administrator of the National Aeronautics and Space Administration. 
**   All Other Rights Reserved.  
**
**   This software was created at NASA's Goddard Space Flight Center.
**   This software is governed by the NASA Open Source Agreement and may be 
**   used, distributed and modified only pursuant to the terms of that 
**   agreement.
**
** Purpose: 
**  The CFS CF Application Cmd header file
**
** Notes:
**
** $Log: cf_cmds.h  $
** Revision 1.6 2015/03/06 14:49:55EST sstrege 
** Added copyright information
** Revision 1.5 2011/05/19 15:32:05EDT rmcgraw 
** DCR15033:1 Add auto suspend processing
** Revision 1.4 2011/05/17 15:52:46EDT rmcgraw 
** DCR14967:5 Message ptr made consistent across all cmds.
** Revision 1.3 2011/05/17 09:25:00EDT rmcgraw 
** DCR14529:1 Added processing for GiveTake Cmd
** Revision 1.2 2010/08/04 15:17:37EDT rmcgraw 
** DCR11510:1 Changes prior to release
** Revision 1.1 2010/07/08 13:06:54EDT rmcgraw 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cf/fsw/src/project.pj
**
*************************************************************************/
#ifndef _cf_cmds_h_
#define _cf_cmds_h_


/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "cf_app.h"
#include "cf_msg.h"
#include "cf_defs.h"
#include "cf_tbldefs.h"
#include "cfdp_config.h"
#include "cf_platform_cfg.h"
#include "cfdp_data_structures.h"




/************************************************************************/
/** \brief Process housekeeping request
**  
**  \par Description
**       Processes an on-board housekeeping request message.
**
**  \par Assumptions, External Events, and Notes:
**       This command does not affect the command execution counter, but 
**       this command will increment the cmd error counter if an invalid cmd
**       length is detected.
**
*************************************************************************/
void CF_HousekeepingCmd (CFE_SB_MsgPtr_t MessagePtr);


/************************************************************************/
/** \brief Process noop command
**  
**  \par Description
**       Processes a noop ground command.
**
**  \par Assumptions, External Events, and Notes:
**       None 
**
**  \sa #CF_NOOP_CC
**
*************************************************************************/
void CF_NoopCmd (CFE_SB_MsgPtr_t MessagePtr);


/************************************************************************/
/** \brief Process reset counters command
**  
**  \par Description
**       Processes a reset counters ground command which will reset
**       the memory manager commmand error and command execution counters
**       to zero.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \sa #CF_RESET_CC
**
*************************************************************************/
void CF_ResetCtrsCmd (CFE_SB_MsgPtr_t MessagePtr);


/************************************************************************/
/** \brief  Verify length of CF commands
**  
**  \par Description
**       Function called when an CF command is received. 
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**  \param [in]   ExpectedLength   The expected lenght of the command
**
**  \returns
**  \retcode #CF_SUCCESS  if actual cmd length is same as expected
**  \retcode #CF_BAD_MSG_LENGTH_RC if actual cmd length is not as expected
**  \endreturns
**
**  \sa 
**
*************************************************************************/
int32 CF_VerifyCmdLength (CFE_SB_MsgPtr_t MessagePtr,uint32 ExpectedLength);
void CF_FileWriteByteCntErr(char *Filename,uint32 Requested,uint32 Actual);
void CF_IncrCmdCtr(int32 Status);
void CF_FreezeCmd(CFE_SB_MsgPtr_t MessagePtr);
void CF_ThawCmd(CFE_SB_MsgPtr_t MessagePtr);
void CF_CARSCmd(CFE_SB_MsgPtr_t MessagePtr, char *WhichCmd);
void CF_SetMibCmd(CFE_SB_MsgPtr_t MessagePtr);
void CF_GetMibCmd(CFE_SB_MsgPtr_t MessagePtr);
void CF_WriteQueueCmd(CFE_SB_MsgPtr_t MessagePtr);
int32 CF_WriteQueueInfo(char *Filename,CF_QueueEntry_t *QueueEntryPtr);
void CF_SendTransDataCmd(CFE_SB_MsgPtr_t MessagePtr);
void CF_WriteActiveTransCmd(CFE_SB_MsgPtr_t MessagePtr);
int32 CF_WriteActiveTransInfo(char *Filename, uint32 WhichQueues);
int32 CF_WrQEntrySubset(char *Filename,int32 Fd,CF_QueueEntry_t *QueueEntryPtr);
void CF_DequeueNodeCmd(CFE_SB_MsgPtr_t MessagePtr);
void CF_SendCfgParams(CFE_SB_MsgPtr_t MessagePtr);
void CF_SetPollParam(CFE_SB_MsgPtr_t MessagePtr);
void CF_PurgeQueueCmd(CFE_SB_MsgPtr_t MessagePtr);
void CF_EnablePollCmd(CFE_SB_MsgPtr_t MessagePtr);
void CF_DisablePollCmd(CFE_SB_MsgPtr_t MessagePtr);
void CF_EnableDequeueCmd(CFE_SB_MsgPtr_t MessagePtr);
void CF_DisableDequeueCmd(CFE_SB_MsgPtr_t MessagePtr);
void CF_KickstartCmd(CFE_SB_MsgPtr_t MessagePtr);
void CF_QuickStatusCmd(CFE_SB_MsgPtr_t MessagePtr);
void CF_GiveTakeSemaphoreCmd(CFE_SB_MsgPtr_t MessagePtr);
void CF_AutoSuspendEnCmd(CFE_SB_MsgPtr_t MessagePtr);
void CF_CheckForTblRequests(void);



#endif /* _cf_cmds_ */

/************************/
/*  End of File Comment */
/************************/
