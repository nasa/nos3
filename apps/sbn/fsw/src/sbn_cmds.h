#ifndef _sbn_cmds_h_
#define _sbn_cmds_h_

#include "cfe.h"

/*************************************************************************
** Function Prototypes
*************************************************************************/


/************************************************************************/
/** \brief Process a command pipe message
**
**  \par Description
**       Processes a single software bus command pipe message.  Checks
**       the message and command IDs and calls the appropriate routine
**       to handle the message.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message
**
**  \sa #CFE_SB_RcvMsg
**
*************************************************************************/
void SBN_AppPipe(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Reset telemetry counters to zero
**
**  \par Description
**       Resets all command and message counters for the SBN app and the 
**       modules to zero. 
**
**  \par Assumptions, External Events, and Notes:
**       Does NOT reset the subscription counter because that counter 
**       is maintained and used outside of the housekeeping functions 
**       and should not change regularly.
**
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message
**
**  \sa #SBN_ResetCountersCmd
**
*************************************************************************/
void SBN_InitializeCounters(void);

#endif /* _sbn_cmds_h_ */
