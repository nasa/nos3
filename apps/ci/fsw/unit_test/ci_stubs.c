/*
 * File: ci_stubs.c
 *
 * Purpose:
 *  Stub out various functions not stubbed out by the UT-Assert code, including standard socket library functions
 */

#include <sys/socket.h>
#include <string.h>
#include <stdio.h>

#include "cfe.h"
#include "ci_app.h"
#include "ci_stubs.h"


extern CI_AppData_t  g_CI_AppData;

Ut_CI_ReturnCodeTable_t     Ut_CI_ReturnCodeTable[UT_CI_MAX_INDEX];

void Ut_CI_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt)
{
    if (Index < UT_CI_MAX_INDEX) {
        Ut_CI_ReturnCodeTable[Index].Value = RtnVal;
        Ut_CI_ReturnCodeTable[Index].Count = CallCnt;
    }
    else {
        printf("Unsupported Index In SetReturnCode Call %u\n", Index);
    }
}


boolean Ut_CI_UseReturnCode(uint32 Index)
{
    if (Ut_CI_ReturnCodeTable[Index].Count > 0) {
        Ut_CI_ReturnCodeTable[Index].Count--;
        if (Ut_CI_ReturnCodeTable[Index].Count == 0)
            return(TRUE);
    }

    return(FALSE);
}



/* Functions normally declared in Custom File (ci_custom.c) */
int32 CI_CustomInit(void)
{
    if (Ut_CI_UseReturnCode(UT_CI_CUSTOMINIT_INDEX))
        return Ut_CI_ReturnCodeTable[UT_CI_CUSTOMINIT_INDEX].Value;
    
    return CI_SUCCESS;
}


int32 CI_CustomAppCmds(CFE_SB_MsgPtr_t pCmdMsg)
{
    uint32 uiCmdCode = CFE_SB_GetCmdCode(pCmdMsg);

    if (Ut_CI_UseReturnCode(UT_CI_CUSTOMAPPCMDS_INDEX))
        return Ut_CI_ReturnCodeTable[UT_CI_CUSTOMAPPCMDS_INDEX].Value;

    CI_IncrHkCounter(&g_CI_AppData.HkTlm.usCmdCnt);
    CFE_EVS_SendEvent(CI_CMD_INF_EID, CFE_EVS_INFORMATION,
                      "Received Custom Cmd (%d)",
                      uiCmdCode);
    
    return CI_SUCCESS;
}


void CI_CustomEnableTO(CFE_SB_MsgPtr_t pCmdMsg)
{
    return;
}


void CI_CustomCleanup(void)
{
    return;
}
