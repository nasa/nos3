/******************************************************************************/
/** \file  ci_utils.c
*  
*   \author Guy de Carufel (Odyssey Space Research), NASA, JSC, ER6
*
*   \brief Function Definitions of utility functions
*
*   \par
*       This file defines utility functions used by other CI functions.
*
*   \par API Functions Defined:
*     - CI_IncrHkCounter() - Increment a HK Counter with memory protection
*     - CI_VerifyCmdLength() - Verify length of command message
*
*   \par Private Functions Defined:
*
*   \par Limitations, Assumptions, External Events, and Notes:
*
*   \par Modification History:
*     - 2015-01-09 | Guy de Carufel | Code Started
*******************************************************************************/

/*
** Include Files
*/
#include "ci_app.h"

/*
** Local Defines
*/

/*
** Local Structure Declarations
*/


/*
** External Global Variables
*/
extern CI_AppData_t g_CI_AppData;

/*
** Global Variables
*/

/*
** Local Variables
*/

/*
** Local Function Definitions
*/


/******************************************************************************/
/** \brief Increment a housekeeping packet counter
*******************************************************************************/
void CI_IncrHkCounter(uint16 * counter)
{
    OS_MutSemTake(g_CI_AppData.ciMutex);
    *counter = *counter + 1;
    OS_MutSemGive(g_CI_AppData.ciMutex);
}


/******************************************************************************/
/** \brief Verify the command length against expected length
*******************************************************************************/
boolean CI_VerifyCmdLength(CFE_SB_MsgPtr_t pMsg,
                           uint16 usExpectedLen)
{
    boolean bResult=FALSE;
    uint16  usMsgLen=0;

    if (pMsg != NULL)
    {
        usMsgLen = CFE_SB_GetTotalMsgLength(pMsg);

        if (usExpectedLen == usMsgLen)
        {
            bResult = TRUE;
        }
        else
        {
            CFE_SB_MsgId_t MsgId = CFE_SB_GetMsgId(pMsg);
            uint16 usCmdCode = CFE_SB_GetCmdCode(pMsg);

            CFE_EVS_SendEvent(CI_MSGLEN_ERR_EID, CFE_EVS_ERROR,
                              "CI: Rcvd invalid msgLen: msgId=0x%04X, "
                              "cmdCode=%d, msgLen=%d, expectedLen=%d",
                              MsgId, usCmdCode, usMsgLen, usExpectedLen);
                              
            CI_IncrHkCounter(&g_CI_AppData.HkTlm.usCmdErrCnt);
        }
    }

    return (bResult);
}

/*==============================================================================
** End of file ci_utils.c
**============================================================================*/
