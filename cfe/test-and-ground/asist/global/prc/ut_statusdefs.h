;PROCOFF

;*******************************************************************************
;  Test Name:  N/A
;  Test Level: Build Verification
;  Test Type:  N/A
;  Test Description
;      This include file defines global status parameters used by all
;      SDO utility procedures
;
;  Requirements Tested
;      None
;
;  Assumptions and Constraints
;      None
;
;  Change History
;     Date           Name             Description
;   08/15/02        C. Firman        Original version
;   10/04/04        R. Coon          Added UT_TBL_Miscompares
;   11/18/04        R. Coon          Added global parameters
;                                     UT_MemSaveByteVal, UT_MemSaveLWordVal,
;                                     UT_MemSaveWordVal, UT_MDMP_Status,
;                                     UT_MVER_Status,    UT_MVER_Miscompares,
;                                     UT_HWWR_Status,    UT_Status,
;                                     UT_TW_Status,      UT_TW_Time,
;
;
;  Arguments
;     Name              Description
;     None
;
;  Procedures Called
;     Name              Description
;     None
;
;  Expected Test Results and Analysis
;     N/A
;
;*******************************************************************************


; Status values
; ******************generic
#define UT_Success               1
#define UT_Failure               2
;
; Telemetry wait status
;
#define UT_TW_Success               1
#define UT_TW_Failure               2


; ******************telemetry update routine
#define UT_TU_TlmUpdated         1
#define UT_TU_TlmNotUpdating     2
#define UT_TU_ParameterError     3
;
; *****************send command routines
#define UT_SC_Success            1
#define UT_SC_TlmNotUpdating     2
#define UT_SC_CmdDropped         3
#define UT_SC_CmdFailure         4
#define UT_SA_Success            1
#define UT_SA_TlmNotUpdating     2
#define UT_SA_CmdDropped         3
#define UT_SA_CmdFailure         4
#define UT_SBC_Success           1
#define UT_SBC_CmdDropped        2
#define UT_SBC_CmdFailure        3
#define UT_SBC_TlmNotUpdating    4
#define UT_SRC_Success           1
#define UT_SRC_CmdDropped        2
#define UT_SRC_CmdFailure        3
#define UT_SRC_TlmNotUpdating    4
#define UT_RCS_Success           1
#define UT_RCS_TlmNotUpdating    2
#define UT_RCS_CmdDropped        3
#define UT_RCS_CmdFailure        4

; *****************restart routines
#define UT_WS_Success            1
#define UT_WS_Failure            2
#define UT_CS_Success            1
#define UT_WS_Failure            2
#define UT_PO_Success            1
#define UT_PO_Failure            2
#define UT_WDT_Success           1
#define UT_WDT_Failure           2
;
; ***************hardware read routines
#define UT_HWRB_Success          1
#define UT_HWRB_Failure          2
#define UT_HWRW_Success          1
#define UT_HWRW_Failure          2
#define UT_HWRL_Success          1
#define UT_HWRL_Failure          2
;
;***************sample value test routine
#define UT_SVT_Success           1
#define UT_SVT_Failure           2
;
; ************* STOL interface routines
#define UT_STOLC_Success         1
#define UT_STOLC_Failure         2
#define UT_RPS_Success           1
#define UT_RPS_Failure           2
;
; ************* Table dump
#define UT_DTBL_Success          1
#define UT_DTBL_Failure          2
;
;**************pass/fail indicate routine
#define UT_PF_Success            1
#define UT_PF_Failure            2
;
;************** warm/cold/softpower on status
#define UT_RST_Success           1
#define UT_RST_Failure           2
#define UT_PWS_Success           1
#define UT_PWS_Cnt_Failure       2
#define UT_PWS_TlmNotUpdating    3
#define UT_PCS_Success           1
#define UT_PCS_Cnt_Failure       2
#define UT_PCS_TlmNotUpdating    3
#define UT_PSPO_Success          1
#define UT_PSPO_Cnt_Failure      2
#define UT_PSPO_TlmNotUpdating   3
;
;************** load rts, ats, and table status
#define UT_LTBL_Success          1
#define UT_LTBL_Failure          2
#define UT_LRTS_Success          1
#define UT_LRTS_Failure          2
#define UT_LATS_Success          1
#define UT_LATS_Failure          2
#define UT_PTL_Success           1
#define UT_PTL_Failure           2
;
; ********** dump critical event table status
#define UT_DCET_Success          1
#define UT_DCET_Failure          2
;
; ********** Mode log dump status
#define UT_MDL_Success           1
#define UT_MDL_Failure           2
;
; ********** Memory dump status
#define UT_MDMP_Success          1
#define UT_MDMP_Failure          2
;
; ********** Memory verification status
#define UT_MVER_Success          1
#define UT_MVER_Failure          2
;
; Global status parameters
;
#ifndef utility
    Global UT_Status
    Global UT_TU_Status
    Global UT_SC_Status
    Global UT_SA_Status
    Global UT_RCS_Status
    Global UT_SBC_Status
    Global UT_SRC_Status
    Global UT_WS_Status
    Global UT_CS_Status
    Global UT_DTBL_Status
    Global UT_WDT_Status
    Global UT_PO_Status
    Global UT_PF_Status
    Global UT_HWRB_Status
    Global UT_HWRW_Status
    Global UT_HWRL_Status
    Global UT_HWWR_Status
    Global UT_SVT_Status
    Global UT_STOLC_Status
    Global UT_RPS_Status
    Global UT_RST_Status
    Global UT_PWS_Status
    Global UT_PCS_Status
    Global UT_PSPO_Status
    Global UT_MemReadByteVal
    Global UT_MemReadWordVal
    Global UT_MemReadLWordVal
    Global UT_MemSaveByteVal
    Global UT_MemSaveWordVal
    Global UT_MemSaveLWordVal
    Global UT_LTBL_Status
    Global UT_LRTS_Status
    Global UT_LATS_Status
    Global UT_MemReadAddress
    Global UT_PTL_Status
    Global UT_MDL_Status
    Global UT_DCET_Status
    Global UT_DCET_RTSErrors
    Global UT_DCET_EVTErrors
    Global UT_DCET_ACTErrors
    Global UT_TBL_Miscompares
    Global UT_MDMP_Status
    Global UT_MVER_Status
    Global UT_MVER_Miscompares
    Global UT_TW_Status
    Global UT_TW_Time
    Global UT_pktrate_cnt[5]
    Global UT_pktrate_avg[5]
    Global UT_pktrate_min[5]
    Global UT_pktrate_max[5]

#endif

;example of usage:
;include "UT_StatusDefs.h"
;UT_TU_Status = UT_TlmUpdate(MCSCMDPC)
;if (UT_TU_Status = UT_TU_TlmUpdated)
;	then this means that the packet was good and updated
;	continue your procedure
;Else if (UT_TU_Status = UT_TU_TlmNotUpdating
;	then this means the data is not updating
;Else if (UT_TU_Status = UT_TU_ParameterError)
; 	the mnemonic is not defined
;Endif
;
;PROCON















