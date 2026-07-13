#include "cfdp_app.h"
#include "cfdp_cmd.h"
#include "cfdp_msg.h"
#include "cfdp_cfdp.h"

#include <string.h>

CFE_Status_t CFDP_NoopCmd()
{
    CFE_EVS_SendEvent(CFDP_CMD_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION, "CFDP: No-Op received, Version %d.%d.%d.%d",
                      CFDP_MAJOR_VERSION, CFDP_MINOR_VERSION, CFDP_REVISION, CFDP_MISSION_REV);

    ++CFDP_AppData.hk.cmdCount;

    return CFE_SUCCESS;
}

/*
** Reset all global counter variables
*/
void CFDP_ResetCounters()
{
    CFDP_AppData.hk.cmdErrorCount = 0;
    CFDP_AppData.hk.cmdCount = 0;
    return;
} 

CFE_Status_t CFDP_SendHkCmd(const CFDP_SendHkCmd_t *msg)
{
    CFE_MSG_SetMsgTime(CFE_MSG_PTR(CFDP_AppData.hk.TlmHeader), CFE_TIME_GetTime());
    /* return value ignored */ CFE_SB_TransmitMsg(CFE_MSG_PTR(CFDP_AppData.hk.TlmHeader), true);

    /* This is also used to check tables */
    //CFDP_CheckTables();

    return CFE_SUCCESS;
}

void CFDP_DownloadFromSatellite(const CFE_MSG_Message_t *MsgPtr)
{
    const CFDP_FileTxSatellite_t *CmdPtr = (const CFDP_FileTxSatellite_t *)MsgPtr; //destination is to ground, source is from satellite

    CFDP_CFDP_SendFile(CmdPtr->Source, CmdPtr->Destination);
}

void CFDP_UploadToSatelliteConfirm(const CFE_MSG_Message_t *MsgPtr)
{
    const CFDP_FileTxSatellite_t *CmdPtr = (const CFDP_FileTxSatellite_t *)MsgPtr; //destination is from ground, source is to satellite
    //CFDP_PrintMe(MsgPtr);

    CFDP_CFDP_SendRequest(CmdPtr->Destination, CmdPtr->Source);
}

void CFDP_UploadToSatellite(const CFE_MSG_Message_t *MsgPtr)
{
    const CFDP_FileSatellite_t *CmdPtr = (const CFDP_FileSatellite_t *)MsgPtr;
    //CFDP_PrintMe(MsgPtr);

    CFDP_CFDP_GetFile(CmdPtr->pdu_type, CmdPtr->buffer, CmdPtr->destination, CmdPtr->length);

}