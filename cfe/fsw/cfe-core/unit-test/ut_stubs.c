/*
**
**      Copyright (c) 2004-2012, United States government as represented by the
**      administrator of the National Aeronautics Space Administration.
**      All rights reserved. This software(cFE) was created at NASA's Goddard
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
** File:
** $Id: ut_stubs.c 1.11 2014/07/10 09:30:58GMT-05:00 rmcgraw Exp  $
**
** Purpose:
** Unit test stubs
**
** Notes:
** Minimal work is done, only what is required for unit testing
**
** $Data:$
** $Revision: 1.11 $
** $Log: ut_stubs.c  $
** Revision 1.11 2014/07/10 09:30:58GMT-05:00 rmcgraw
** DCR9772:2 Fix unit tests that broke when main files were checked in
** Revision 1.10 2014/05/28 10:21:44EDT wmoleski
** Overwriting cFE Unit Test files with the updated JSC files.
** Revision 1.9 2012/01/13 13:59:31EST acudmore
** Added license text
** Revision 1.8 2009/08/04 13:34:27EDT aschoeni
** Added heap information to ES telemetry
** Revision 1.7 2009/07/29 19:23:43EDT aschoeni
** Added GetPoolInfoRtn
** Revision 1.6 2009/05/06 09:55:00EDT rmcgraw
** DCR7366:1 Clear event history in UT_InitData
** Revision 1.5 2009/04/28 16:25:14EDT rmcgraw
** DCR7366:1 Added utility UT_CheckForOpenSockets
** Revision 1.4 2009/04/27 10:39:49EDT rmcgraw
** DCR7366:1 Added printing number of tests that passed in output.
** Revision 1.3 2009/04/01 16:14:10EDT rmcgraw
** DCR7366:1 Added poolCreateEx items and moved some utilities from sb_UT.c
** Revision 1.2 2008/08/15 11:26:55EDT njyanchik
** Check in of ES Unit Test
** Revision 1.1 2008/04/17 13:05:47BST ruperera
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/unit-test/project.pj
** Revision 1.24 2007/09/21 19:02:55EDT David Kobe (dlkobe)
** Updated table unit tests to stop segmentation fault
** Revision 1.23 2007/07/09 15:31:48EDT dlkobe
** Improper order of "memset" parameters was fixed
** Revision 1.22 2007/07/08 22:30:54EDT dlkobe
** Enhanced CDS simulation
** Revision 1.21 2007/07/05 15:04:26EDT dlkobe
** Baseline following complete TBL unit testing
** Revision 1.20 2007/05/30 15:15:22EDT njyanchik
** check in of ES Unit test
** Revision 1.19 2007/05/30 08:56:41EDT njyanchik
** Update
** Revision 1.18 2007/05/25 13:13:43EDT njyanchik
** update of ES unit test
** Revision 1.17 2007/05/24 15:48:09EDT njyanchik
** Update for ES Unit Test
** Revision 1.16 2007/05/22 13:03:58EDT njyanchik
** Updated changes to not lose my work
** Revision 1.15 2007/05/16 11:14:37EDT njyanchik
** Update ES's unit test driver to match code for build 4.1
** Revision 1.14 2007/05/10 15:14:26EDT njyanchik
** Another update of Jonathans UT
** Revision 1.13 2007/05/07 13:45:38EDT njyanchik
** EVS's Unit test drivers have been updated
** Revision 1.12 2007/05/04 09:10:24EDT njyanchik
** Check in of Time UT and related changes
** Revision 1.10 2007/05/01 13:28:13EDT njyanchik
** I updated the ut stubs to get the each of the subsytems to compile under the unit test. I did not
** change the unit tests themselves to cover more of the files, however.
** Revision 1.9 2007/03/02 15:18:05EST njyanchik
** Jonathan provided me with a newer tbl unit test to check in
** Revision 1.8 2007/01/17 09:26:07GMT-05:00 njyanchik
** Check in of ES Unit Test changed files
** Revision 1.7 2006/11/08 15:06:08GMT-05:00 rjmcgraw
** Added FSWriteHdrRtn variable
** Revision 1.6 2006/11/02 13:53:58EST njyanchik
** Unit test for TIME was updated to match the changes made for this DCR
** Revision 1.5 2006/11/01 12:46:54GMT-05:00 njyanchik
** Changed the Unit test to reflect the changes from removing the CDS functionality from TIME
** Revision 1.4 2006/07/26 07:10:41GMT-05:00 rjmcgraw
** Changed SemGiv to SemGive and added CreatePoolRtn
** Revision 1.3 2006/06/08 19:12:37GMT rjmcgraw
** Added support for function UT_SetRtnCode
** Revision 1.2 2006/05/17 13:56:42GMT jjhageman
** Added UT_Queue for use by the osapi stubs (OS_Queue create, get, set)
** Revision 1.1 2006/03/02 15:10:26EST jjhageman
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/fsw/cfe-core/unit-test/project.pj
*/

/*
** Includes
*/
#include <string.h>
#include "ut_stubs.h"

/*
** Global variables
*/
char    UT_appname[80];
char    UT_subsys[5];
char    cMsg[UT_MAX_MESSAGE_LENGTH];
uint8   UT_CDS[UT_CDS_SIZE];
uint32  UT_BSP_Fail = 0x0000;
uint16  UT_RcvMsgCode = 0;
uint32  UT_PutPool_Fail = 0;
uint32  UT_AppID = 0;
uint32  UT_StatusCDS = 0;
uint32  UT_SignatureCDS = 0;
uint32  UT_LocTimeSec = 0;
uint32  UT_LocTimeMSec = 0;
boolean UT_CDS_GoodEnd = TRUE;
boolean UT_BSPCheckValidity = FALSE;
boolean UT_CDS_Rebuild = FALSE;
boolean UT_CDSReadBlock = FALSE;
uint16  UT_EventHistory[UT_EVENT_HISTORY_SIZE];
uint32  UT_EventHistoryCtr = 0;
int32   UT_StatusBSP = OS_SUCCESS;
uint8   UT_Endianess = 0;
uint32  poolBufIndex = 0;
char    UT_ReadHdr[10000];
int32   UT_DummyFuncRtn;
int32   UT_SizeofESResetArea;
int32   UT_ReadHdrOffset;
int32   UT_CDS_Size;
int32   UT_SB_TotalMsgLen;

UT_CDS_Map_t          UT_CDS_Map;
OS_time_t             BSP_Time;
CFE_SB_MsgId_t        UT_RcvMsgId = 0;
CFE_EVS_GlobalData_t  CFE_EVS_GlobalData;
CFE_ES_ResetData_t    UT_CFE_ES_ResetData;
CFE_ES_ResetData_t    *UT_CFE_ES_ResetDataPtr;

UT_SetRtn_t SBSendMsgRtn;
UT_SetRtn_t FSSetTimestampRtn;
UT_SetRtn_t FSWriteHdrRtn;
UT_SetRtn_t FSReadHdrRtn;
UT_SetRtn_t CreatePoolRtn;
UT_SetRtn_t PoolCreateExRtn;
UT_SetRtn_t PutPoolRtn;
UT_SetRtn_t GetPoolRtn;
UT_SetRtn_t GetPoolInfoRtn;
UT_SetRtn_t BSPWriteCDSRtn;
UT_SetRtn_t BSPReadCDSRtn;
UT_SetRtn_t ES_DeleteCDSRtn;
UT_SetRtn_t ES_RegisterCDSRtn;
UT_SetRtn_t ES_CopyToCDSRtn;
UT_SetRtn_t ES_RestoreFromCDSRtn;
UT_SetRtn_t BSPUnloadAppFileRtn;
UT_SetRtn_t GetAppIDRtn;
UT_SetRtn_t BSPGetCFETextRtn;
UT_SetRtn_t FSIsGzFileRtn;
UT_SetRtn_t FSDecompressRtn;
UT_SetRtn_t FSExtractRtn;
UT_SetRtn_t ES_ExitAppRtn;
UT_SetRtn_t ES_RegisterRtn;
UT_SetRtn_t ES_CreateChildRtn;
UT_SetRtn_t SB_SubscribeRtn;
UT_SetRtn_t SB_SubscribeLocalRtn;
UT_SetRtn_t SB_SubscribeExRtn;
UT_SetRtn_t SB_CreatePipeRtn;
UT_SetRtn_t EVS_SendEventRtn;
UT_SetRtn_t EVS_RegisterRtn;
UT_SetRtn_t SBCleanUpRtn;
UT_SetRtn_t EVSCleanUpRtn;
UT_SetRtn_t TIMECleanUpRtn;
UT_SetRtn_t WriteSysLogRtn;
UT_SetRtn_t GetResetTypeRtn;
UT_SetRtn_t ES_GetTaskInfoRtn;
UT_SetRtn_t SendMsgEventIDRtn;
UT_SetRtn_t OSPrintRtn;
UT_SetRtn_t SetMsgIdRtn;
UT_SetRtn_t PSPRestartRtn;
UT_SetRtn_t PSPPanicRtn;
UT_SetRtn_t TBLEarlyInitRtn;
UT_SetRtn_t PSPMemValRangeRtn;

/*
 * Globals for controlling the OSAL UT stubs (deprecated, but still frequently used)
 */
uint32  UT_OS_Fail = 0;
uint32  UT_BinSemFail = 0;

extern  int32 dummy_function(void);

/*
** Functions
*/
/*
** Initialize unit test
*/
void UT_Init(char *subsys)
{
    int8 i;

    /* Copy the application name for later use */
    strncpy(UT_subsys, subsys, 5);
    snprintf(UT_appname, 80, "ut_cfe_%s", subsys);

    /* Convert to upper case */
    for (i = 0; i < strlen(UT_subsys); ++i)
    {
        if (UT_subsys[i] >= 'a' && UT_subsys[i] <= 'z')
        {
            UT_subsys[i] = (UT_subsys[i] - 'a') + 'A';
        }
    }

    UT_InitData();
    UT_EndianCheck();
}

/*
** Initialize unit test variables
*/
void UT_InitData(void)
{
    /*
     * Purge all entries in the entire stub table
     */
    UT_ResetState(0);

    /*
     * Reset the OSAL stubs to the default state
     */
    OS_API_Init();

    UT_ClearEventHistory();
    UT_ResetPoolBufferIndex();
    UT_SetBSPFail(0);
    UT_SetOSFail(0);

    /* Initialize values and counters used for forcing return values
     * from stubs
     */
    UT_SetRtnCode(&CreatePoolRtn, 0, 0);
    UT_SetRtnCode(&PoolCreateExRtn, 0, 0);
    UT_SetRtnCode(&PutPoolRtn, 0, 0);
    UT_SetRtnCode(&GetPoolRtn, 0, 0);
    UT_SetRtnCode(&BSPWriteCDSRtn, 0, 0);
    UT_SetRtnCode(&BSPReadCDSRtn, 0, 0);
    UT_SetRtnCode(&SBSendMsgRtn, 0, 0);
    UT_SetRtnCode(&FSSetTimestampRtn, 0, 0);
    UT_SetRtnCode(&FSWriteHdrRtn, 0, 0);
    UT_SetRtnCode(&FSReadHdrRtn, 0, 0);
    UT_SetRtnCode(&ES_RegisterRtn, 0, 0);
    UT_SetRtnCode(&ES_CreateChildRtn, 0, 0);
    UT_SetRtnCode(&SB_SubscribeRtn, 0, 0);
    UT_SetRtnCode(&SB_SubscribeLocalRtn, 0, 0);
    UT_SetRtnCode(&SB_CreatePipeRtn, 0, 0);
    UT_SetRtnCode(&EVS_SendEventRtn, 0, 0);
    UT_SetRtnCode(&EVS_RegisterRtn, 0, 0);
    UT_SetRtnCode(&SB_SubscribeExRtn, 0, 0);
    UT_SetRtnCode(&BSPUnloadAppFileRtn, 0, 0);
    UT_SetRtnCode(&SBCleanUpRtn, 0, 0);
    UT_SetRtnCode(&EVSCleanUpRtn, 0, 0);
    UT_SetRtnCode(&GetAppIDRtn, 0, 0);
    UT_SetRtnCode(&BSPGetCFETextRtn, 0, 0);
    UT_SetRtnCode(&FSIsGzFileRtn, 0, 0);
    UT_SetRtnCode(&FSDecompressRtn, 0, 0);
    UT_SetRtnCode(&FSExtractRtn, 0, 0);
    UT_SetRtnCode(&TIMECleanUpRtn, 0, 0);
    UT_SetRtnCode(&GetPoolInfoRtn, 0, 0);
    UT_SetRtnCode(&WriteSysLogRtn, -1, 0);
    UT_SetRtnCode(&GetResetTypeRtn, 0, 0);
    UT_SetRtnCode(&ES_GetTaskInfoRtn, 0, 0);
    UT_SetRtnCode(&SendMsgEventIDRtn, -1, 0);
    UT_SetRtnCode(&OSPrintRtn, 0, -1);
    UT_SetRtnCode(&SetMsgIdRtn, 0, 0);
    UT_SetRtnCode(&PSPRestartRtn, 0, 0);
    UT_SetRtnCode(&PSPPanicRtn, 0, 0);
    UT_SetRtnCode(&TBLEarlyInitRtn, 0, 0);
    UT_SetRtnCode(&PSPMemValRangeRtn, 0, 0);
    UT_SetRtnCode(&ES_ExitAppRtn, 0, 0);

    UT_ResetCDS();
}

/*
** Reset pool buffer index
*/
void UT_ResetPoolBufferIndex(void)
{
    poolBufIndex = 0;
}

/*
** Output a text string
*/
void UT_Text(char *text)
{
    UtPrintf("%s\n", text);
}

/*
** Output single test's pass/fail status
*/
void UT_Report(const char *file, uint32 line, boolean test, char *fun_name,
		       char *info)
{
    UtAssertEx(test, UtAssert_GetContext(), file, line, "%s - %s", fun_name,
    		   info);
}

/*
** Send a message via the software bus
*/
void UT_SendMsg(CFE_SB_MsgPtr_t msg_ptr, CFE_SB_MsgId_t id, uint16 code)
{
    CFE_SB_SetMsgId(msg_ptr, id);
    CFE_SB_SetCmdCode(msg_ptr, code);
    CFE_SB_SendMsg(msg_ptr);
}

/*
** Set the application ID returned by unit test stubs
*/
void UT_SetAppID(int32 AppID_in)
{
    UT_AppID = AppID_in;
}

/*
** Set the PutPool fail flag
*/
void UT_SetPutPoolFail(uint32 fail)
{
    UT_PutPool_Fail = fail;
}


/*
 * NOTE - The UT_SetOSFail(), UT_SetRtnCode(), and UT_SetBinSemFail() functions
 * are replaced by implementations within UT assert generic stub library.
 *
 * These are now wrappers to make the tests work until all the test code can
 * be updated to call the functions directly and not use these anymore.
 */

/*
** Set the OS fail flag
*/
void UT_SetOSFail(uint32 fail)
{
    /*
     * This table MUST be in the SAME ORDER as the bitmaps in the header file.
     * This is a translation layer for use until calls to UT_SetOSFail() in
     * the CFE unit tests can be updated.
     */
    static const UT_EntryKey_t FAIL_BITMAPS[] =
    {
            UT_KEY(OS_creat),       /* OS_CREAT_FAIL        0x0001 */
            UT_KEY(OS_write),       /* OS_WRITE_FAIL        0x0002 */
            UT_KEY(OS_close),       /* OS_CLOSE_FAIL        0x0004 */
            UT_KEY(OS_mkfs),        /* OS_MKFS_FAIL         0x0008 */
            UT_KEY(OS_initfs),      /* OS_INITFS_FAIL       0x0010 */
            UT_KEY(OS_mount),       /* OS_MOUNT_FAIL        0x0020 */
            UT_KEY(OS_TaskCreate),  /* OS_TASKCREATE_FAIL   0x0040 */
            UT_KEY(OS_BinSemCreate),/* OS_SEMCREATE_FAIL    0x0080 */
            UT_KEY(OS_MutSemCreate),/* OS_MUTCREATE_FAIL    0x0100 */
            UT_KEY(OS_open),        /* OS_OPEN_FAIL         0x0200 */
            UT_KEY(OS_TaskDelay),   /* OS_TASKDELAY_FAIL    0x0400 */
            UT_KEY(OS_TaskRegister),/* OS_TASKREGISTER_FAIL 0x0800 */
            UT_KEY(OS_read),        /* OS_READ_FAIL         0x1000 */
            UT_KEY(OS_lseek),       /* OS_LSEEK_FAIL        0x2000 */
            UT_KEY(OS_TaskDelete),  /* OS_TASKDELETE_FAIL   0x4000 */
            UT_KEY(OS_rmfs),        /* OS_RMFS_FAIL         0x8000 */
            0                       /* End of table */
    };

    const UT_EntryKey_t *KeyPtr;
    uint32 BitMask;

    BitMask = fail;
    KeyPtr = FAIL_BITMAPS;
    while (*KeyPtr != 0)
    {
        if (BitMask & 0x1)
        {
            UT_SetForceFail(*KeyPtr, OS_ERROR);
        }
        else
        {
            UT_ClearForceFail(*KeyPtr);
        }
        ++KeyPtr;
        BitMask >>= 1;
    }

    UT_OS_Fail = fail;
}

/*
** Generic function used to tell stub functions to return 'rtnVal' when
** call 'cnt' = 0
*/
void UT_SetRtnCode(UT_SetRtn_t *varPtr, int32 rtnVal, int32 cnt)
{
    UT_Compat_SetRtnCode(varPtr, rtnVal, cnt);
    varPtr->value = rtnVal;
    varPtr->count = cnt;
}

/*
** Set the BinSem fail flag
*/
void UT_SetBinSemFail(uint32 fail)
{
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), fail, OS_ERROR);
    UT_BinSemFail = fail;
}

/*
** Set the return code of CFE_PSP_GetResetArea and the data validation
** signature
*/
void UT_SetStatusBSPResetArea(int32 status, uint32 Signature, uint32 ClockSignal)
{
    UT_CFE_ES_ResetData.TimeResetVars.Signature = Signature;
    UT_CFE_ES_ResetData.TimeResetVars.ClockSignal = ClockSignal;
    UT_StatusBSP = status;
}

/*
** Set the contents of the buffer to read
*/
void UT_SetReadBuffer(void *Buff, int NumBytes)
{
    UT_SetDataBuffer(UT_KEY(OS_read), Buff, NumBytes, TRUE);
}

/*
** Set the contents of the header to read
*/
void UT_SetReadHeader(void *Hdr, int NumBytes)
{
    UT_ReadHdrOffset = 0;
    memcpy(&UT_ReadHdr, Hdr, NumBytes);
}

/*
** Set the dummy function return code
*/
void UT_SetDummyFuncRtn(int Return)
{
    UT_SetForceFail(UT_KEY(dummy_function), Return);
}

/*
** Set the size of the ES reset area
*/
void UT_SetSizeofESResetArea(int32 Size)
{
    UT_SizeofESResetArea = Size;
}

/*
** Set the failure mode for various BSP stub functions
*/
void UT_SetBSPFail(uint32 Fail)
{
    UT_BSP_Fail = Fail;
}

/*
** Set the CDS size returned by the BSP
*/
void UT_SetCDSSize(int32 Size)
{
    UT_CDS_Size = Size;
}

/*
** Set the SB total message length
*/
void UT_SetSBTotalMsgLen(int Size)
{
    UT_SB_TotalMsgLen = Size;
}

/*
** Set the CDS valid end flag
*/
void UT_SetCDSReadGoodEnd(boolean Truth)
{
    UT_CDS_GoodEnd = Truth;
}

/*
** Set the BSP check validity flag
*/
void UT_SetCDSBSPCheckValidity(boolean Truth)
{
    UT_BSPCheckValidity = Truth;
}

/*
** Set the CDS rebuild flag
*/
void UT_SetCDSRebuild(boolean Truth)
{
    UT_CDS_Rebuild = Truth;
}

/*
** Set BSP time
*/
void UT_SetBSP_Time(uint32 seconds, uint32 microsecs)
{
    BSP_Time.seconds = seconds;
    BSP_Time.microsecs = microsecs;
}

/*
** Clear the event history
*/
void UT_ClearEventHistory(void)
{
    uint32 i = 0;

    for (i = 0; i < UT_EVENT_HISTORY_SIZE; i++)
    {
        UT_EventHistory[i] = 0;
    }

    UT_EventHistoryCtr = 0;
}

/*
** Add an event ID to the event history
*/
void UT_AddEventToHistory(uint16 EventID)
{
    uint16 Index = UT_EventHistoryCtr;

    /* Wrap Index if too many events have been sent */
    Index = UT_EventHistoryCtr % UT_EVENT_HISTORY_SIZE;
    UT_EventHistory[Index] = EventID;
    UT_EventHistoryCtr++;
}

/*
** Search the event history for a specified event ID
*/
boolean UT_EventIsInHistory(uint16 EventIDToSearchFor)
{
    boolean Result = FALSE;
    uint32 i = 0;

    for (i = 0; ((i < UT_EVENT_HISTORY_SIZE) && (i < UT_EventHistoryCtr)); i++)
    {
        if (UT_EventHistory[i] == EventIDToSearchFor)
        {
            i = UT_EVENT_HISTORY_SIZE;
            Result = TRUE;
        }
    }

    return Result;
}

/*
** Return number of events issued
*/
uint16 UT_GetNumEventsSent(void)
{
    return UT_EventHistoryCtr;
}

/*
** Initialize the CDS
*/
void UT_ResetCDS(void)
{
    ES_DeleteCDSRtn.count = 0;
    ES_RegisterCDSRtn.count = 0;
    ES_CopyToCDSRtn.count = 0;
    ES_RestoreFromCDSRtn.count = 0;
    memset(UT_CDS, 0, sizeof(UT_CDS));
    memset(&UT_CDS_Map.NextHandle, 0, sizeof(UT_CDS_Map));
}

/*
** Perform a test to determine endianess
*/
void UT_EndianCheck(void)
{
    int32 EndianCheck = 0x01020304;

    if ((*(char *) &EndianCheck) == 0x04)
    {
        UT_Endianess = UT_LITTLE_ENDIAN;
    }
    else
    {
        UT_Endianess = UT_BIG_ENDIAN;
    }
}

/*
** Display the contents of a packet
*/
void UT_DisplayPkt(CFE_SB_MsgPtr_t ptr, uint32 size)
{
    uint8 *BytePtr = (uint8 *) ptr;
    uint32 i;
    int delta;
    char *msgPtr = cMsg;

    cMsg[0] = '\0';

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        /* Little endian */
        BytePtr += size;
        delta = -1;
    }
    else
    {
        /* Big endian */
        delta = 1;
    }

    for (i = 0; i < size; i++)
    {
        snprintf(msgPtr, UT_MAX_MESSAGE_LENGTH, "%02x ", *BytePtr);
        msgPtr += 3;
        BytePtr += delta;
    }

    UT_Text(cMsg);
}

/*
** Return the actual packet length
*/
int16 UT_GetActualPktLenField(CFE_SB_MsgPtr_t MsgPtr)
{

    CCSDS_PriHdr_t *Ptr;

    Ptr = (CCSDS_PriHdr_t *) MsgPtr;

    return (Ptr->Length[0] << 8) + Ptr->Length[1];

}

/*
** Return the actual command code
*/
uint8 UT_GetActualCmdCodeField(CFE_SB_MsgPtr_t MsgPtr)
{
    /*
     * CFE 6.4.0 tried to make all headers big-endian.
     * CFE 6.4.1 made secondary headers native-endian again.
     *
     * This function is used to "go around" the structure field
     * definitions and access macro definitions, to look for the
     * bits of the function code in the exact spot where we are
     * expecting to find them.
     *
     * The CCSDS Command Function Code is defined as living in
     * bits 8 through 14 (mask 0x7F00) of the 16-bit unsigned
     * value encoded in NATIVE endianness in the two bytes
     * stored at offsets 6 and 7 in the packet.
     */

    uint16 *w = (uint16 *)MsgPtr;

    return (w[3] & 0x7F00) >> 8;
}

/*
** Compare two strings.  The tilde (~) is used as a wildcard in the second
** string and is considered to match the associated numeric character(s) in the
** first string (includes hexadecimal values).  Returns FALSE (0) if the
** strings do not match; otherwise returns TRUE (1)
*/
uint8 UT_strcmp(char *str1, char *str2)
{
    char *ptr1, *ptr2;
    int stringsMismatch = TRUE;

    /* Step through each character in both strings */
    for (ptr1 = str1, ptr2 = str2;
         *ptr1 != '\0' && *ptr2 != '\0';
         ptr1++, ptr2++)
    {
        /* Check if the characters don't match */
        if (*ptr1 != *ptr2)
        {
            /* Check if string 2's character is a wildcard character */
            if (*ptr2 == '~')
            {
                /* Check if string 1's character is a number (including
                 * hexadecimal)
                 */
                if (isHex(*ptr1))
                {
                    /* Continue to check if the next character in string 1 is a
                     * number.  Leave the pointer at the last numeric character
                     */
                    while (isHex(*(ptr1 + 1)))
                    {
                        /* Increment string 1's pointer */
                        ptr1++;
                    }

                    /* Skip past consecutive wildcard characters in string 2 */
                    while (*(ptr2 + 1) == '~')
                    {
                        ptr2++;
                    }
                }
                /* No number to match with the wildcard character */
                else
                {
                    /* Exit the loop (indicates a mismatch) */
                    break;
                }
            }
            /* Not a wildcard character to match to a number */
            else
            {
                /* Exit the loop (indicates a mismatch) */
                break;
            }
        }
    }

    /* Check if the end of both strings was reached */
    if (*ptr1 == '\0' && *ptr2 == '\0')
    {
        /* Comparison completed without a mismatch */
        stringsMismatch = FALSE;
    }

    return stringsMismatch;
}
