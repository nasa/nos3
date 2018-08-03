/************************************************************************
** File:
**   $Id: cf_events.h 1.20.1.1 2015/03/06 15:30:33EST sstrege Exp  $
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
**  The CFS CF Application event id header file
**
** Notes:
**
** $Log: cf_events.h  $
** Revision 1.20.1.1 2015/03/06 15:30:33EST sstrege 
** Added copyright information
** Revision 1.20 2011/05/19 15:32:05EDT rmcgraw 
** DCR15033:1 Add auto suspend processing
** Revision 1.19 2011/05/17 17:22:25EDT rmcgraw 
** DCR14967:11 In Mach Alloc, send event if mem allocation fails
** Revision 1.18 2011/05/17 17:04:47EDT rmcgraw 
** DCR14967:9 removed parameter checks in QueueDirectoryFiles
** Revision 1.17 2011/05/17 09:25:08EDT rmcgraw 
** DCR14529:1 Added processing for GiveTake Cmd
** Revision 1.16 2011/05/10 14:38:40EDT rmcgraw 
** Removed event id 25 in CF_MoveDwnNodeActiveToHistory
** Revision 1.15 2011/05/10 10:46:21EDT rmcgraw 
** DCR14967:2 Added error event 66, chan param err in write queue cmd
** Revision 1.14 2011/05/03 16:47:23EDT rmcgraw 
** Cleaned up events, removed \n from some events, removed event id ganging
** Revision 1.13 2010/11/01 16:09:35EDT rmcgraw 
** DCR12802:1 Changes for decoupling peer entity id from channel
** Revision 1.12 2010/08/04 15:17:38EDT rmcgraw 
** DCR11510:1 Changes prior to release
** Revision 1.11 2010/07/20 14:37:41EDT rmcgraw 
** Dcr11510:1 Remove Downlink buffer references
** Revision 1.10 2010/07/08 13:47:33EDT rmcgraw 
** DCR11510:1 Added termination checking on all cmds that take a string
** Revision 1.9 2010/07/07 17:37:43EDT rmcgraw 
** DCR11510:1 Added events
** Revision 1.8 2010/06/11 16:13:17EDT rmcgraw 
** DCR11510:1 ZeroCopy, Un-hardcoded cmd/tlm input/output pdus
** Revision 1.7 2010/06/01 10:57:13EDT rmcgraw 
** DCR11510:1 Removed spares and added new event ids
** Revision 1.6 2010/05/24 14:06:38EDT rmcgraw 
** DCR11510:1 Added Event Id for table load attempt
** Revision 1.5 2010/04/23 08:39:17EDT rmcgraw 
** Dcr11510:1 Code Review Prep
** Revision 1.4 2010/03/26 15:30:23EDT rmcgraw 
** DCR11510 Various developmental changes
** Revision 1.3 2010/03/12 12:14:36EST rmcgraw 
** DCR11510:1 Initial check-in towards CF Version 1000
** Revision 1.2 2009/12/08 09:12:41EST rmcgraw 
** DCR10350:3 Set poll dir name error events added
** Revision 1.1 2009/11/24 12:48:53EST rmcgraw 
** Initial revision
** Member added to CFS CF project
**
*************************************************************************/
#ifndef _cf_events_h_
#define _cf_events_h_


/*************************************************************************
** Macro definitions
**************************************************************************/


#define CF_INIT_EID                 1
#define CF_CC_ERR_EID               2
#define CF_MID_ERR_EID              3
#define CF_CMD_LEN_ERR_EID          4
#define CF_NOOP_CMD_EID             5
#define CF_RESET_CMD_EID            6
#define CF_FILE_IO_ERR1_EID			    7
#define CF_CR_PIPE_ERR_EID			    8
#define CF_SUB_REQ_ERR_EID			    9
#define CF_SUB_CMD_ERR_EID			    10
#define CF_RCV_MSG_ERR_EID		      11
#define CF_FILE_IO_ERR2_EID         12
#define CF_REMOVE_ERR1_EID          13
#define CF_LOGIC_NAME_ERR_EID       14
#define CF_CFDP_ENGINE_DEB_EID      15
#define CF_CFDP_ENGINE_INFO_EID     16
#define CF_CFDP_ENGINE_WARN_EID     17
#define CF_CFDP_ENGINE_ERR_EID      18
#define CF_FILE_IO_ERR3_EID         19
#define CF_IN_TRANS_OK_EID          20
#define CF_OUT_TRANS_OK_EID         21
#define CF_IN_TRANS_FAILED_EID      22
#define CF_OUT_TRANS_FAILED_EID     23
#define CF_MV_UP_NODE_EID           24
#define CF_ENDIS_AUTO_SUS_CMD_EID   25
#define CF_IND_XACT_SUS_EID         26
#define CF_IND_XACT_RES_EID         27
#define CF_IND_XACT_FAU_EID         28
#define CF_IND_XACT_ABA_EID         29
#define CF_KICKSTART_CMD_EID        30
#define CF_REMOVE_ERR2_EID          31  
#define CF_IND_ACK_TIM_EXP_EID      32
#define CF_IND_INA_TIM_EXP_EID      33
#define CF_IND_NACK_TIM_EXP_EID     34
#define CF_IND_UNEXP_TYPE_EID       35
#define CF_FILE_CLOSE_ERR_EID       36
#define CF_MACH_ALLOC_ERR_EID       37
#define CF_PLAYBACK_FILE_EID        38
#define CF_PUT_REQ_ERR1_EID         39
#define CF_PUT_REQ_ERR2_EID         40
#define CF_CFGTBL_REG_ERR_EID       41
#define CF_CFGTBL_LD_ERR_EID        42
#define CF_CFGTBL_MNG_ERR_EID       43
#define CF_CFGTBL_GADR_ERR_EID      44
#define CF_TRANS_SUSPEND_OVRFLW_EID 45
#define CF_FREEZE_CMD_EID           46
#define CF_THAW_CMD_EID             47
#define CF_CARS_CMD_EID             48
#define CF_CARS_ERR1_EID            49
#define CF_SET_MIB_CMD_EID          50
#define CF_GET_MIB_CMD_EID          51
#define CF_SUB_PDUS_ERR_EID         52
#define CF_SND_Q_INFO_EID           53
#define CF_FILEWRITE_ERR_EID        54
#define CF_SUB_WAKE_ERR_EID         55
#define CF_KICKSTART_ERR1_EID       56
#define CF_WR_CMD_ERR1_EID          57
#define CF_WR_CMD_ERR2_EID          58
#define CF_WR_CMD_ERR3_EID          59
#define CF_SND_QUE_ERR1_EID         60
#define CF_SND_TRANS_ERR_EID        61
#define CF_DEQ_NODE_ERR1_EID        62
#define CF_DEQ_NODE_ERR2_EID        63
#define CF_DEQ_NODE_ERR3_EID        64
#define CF_DEQ_NODE_ERR4_EID        65
#define CF_WR_CMD_ERR4_EID          66
/* 67 unused */
#define CF_DEQ_NODE1_EID            68
#define CF_DEQ_NODE2_EID            69
#define CF_PDU_RCV_ERR1_EID         70
#define CF_PDU_RCV_ERR2_EID         71
#define CF_PDU_RCV_ERR3_EID         72
#define CF_HANDSHAKE_ERR1_EID       73
#define CF_SND_TRANS_CMD_EID        74
#define CF_IND_FAU_UNEX_EID         75
#define CF_PB_FILE_ERR1_EID         76
#define CF_PB_FILE_ERR2_EID         77
#define CF_PB_FILE_ERR3_EID         78
#define CF_PB_FILE_ERR4_EID         79
#define CF_PB_FILE_ERR5_EID         80
#define CF_PB_FILE_ERR6_EID         81
#define CF_QDIR_INV_NAME1_EID       82
#define CF_QDIR_INV_NAME2_EID       83
#define CF_MEM_ALLOC_ERR_EID        84
#define CF_MEM_DEALLOC_ERR_EID      85
#define CF_ENA_DQ_CMD_EID           86
#define CF_DQ_CMD_ERR1_EID          87
#define CF_DIS_DQ_CMD_EID           88
#define CF_DQ_CMD_ERR2_EID          89
#define CF_ENA_POLL_CMD1_EID        90
#define CF_ENA_POLL_CMD2_EID        91
#define CF_ENA_POLL_ERR1_EID        92
#define CF_ENA_POLL_ERR2_EID        93
#define CF_DIS_POLL_CMD1_EID        94
#define CF_DIS_POLL_CMD2_EID        95
#define CF_DIS_POLL_ERR1_EID        96
#define CF_DIS_POLL_ERR2_EID        97
#define CF_OPEN_DIR_ERR_EID         98
#define CF_QDIR_NOMEM1_EID          99
#define CF_QDIR_NOMEM2_EID          100
#define CF_QDIR_PQFUL_EID           101
#define CF_IN_TRANS_START_EID       102
#define CF_OUT_TRANS_START_EID      103
#define CF_SET_MIB_CMD_ERR1_EID     104
#define CF_SET_MIB_CMD_ERR2_EID     105
#define CF_TBL_VAL_ERR1_EID         106
#define CF_TBL_VAL_ERR2_EID         107
#define CF_TBL_VAL_ERR3_EID         108
#define CF_TBL_VAL_ERR4_EID         109
#define CF_TBL_VAL_ERR5_EID         110
#define CF_TBL_VAL_ERR6_EID         111
#define CF_TBL_VAL_ERR7_EID         112
#define CF_TBL_VAL_ERR8_EID         113
#define CF_TBL_VAL_ERR9_EID         114
#define CF_TBL_VAL_ERR10_EID        115
#define CF_TBL_VAL_ERR11_EID        116
#define CF_TBL_VAL_ERR12_EID        117
#define CF_TBL_VAL_ERR13_EID        118
#define CF_TBL_VAL_ERR14_EID        119
#define CF_NO_TERM_ERR_EID          120
#define CF_SET_POLL_PARAM_ERR1_EID  121
#define CF_SET_POLL_PARAM_ERR2_EID  122
#define CF_SET_POLL_PARAM_ERR3_EID  123
#define CF_SET_POLL_PARAM_ERR4_EID  124
#define CF_SET_POLL_PARAM_ERR5_EID  125
#define CF_SET_POLL_PARAM_ERR6_EID  126
#define CF_SET_POLL_PARAM_ERR7_EID  127
#define CF_SET_POLL_PARAM1_EID      128
#define CF_SND_CFG_CMD_EID          129
#define CF_GIVETAKE_ERR1_EID        130
#define CF_GIVETAKE_ERR2_EID        131
#define CF_GIVETAKE_ERR3_EID        132
#define CF_GIVETAKE_ERR4_EID        133
#define CF_PLAYBACK_DIR_EID         134
#define CF_PB_DIR_ERR1_EID          135
#define CF_PB_DIR_ERR2_EID          136
#define CF_PB_DIR_ERR3_EID          137
#define CF_PB_DIR_ERR4_EID          138
#define CF_PB_DIR_ERR5_EID          139
#define CF_PURGEQ_ERR1_EID          140
#define CF_PURGEQ_ERR2_EID          141
#define CF_PURGEQ_ERR3_EID          142
#define CF_PURGEQ_ERR4_EID          143
#define CF_PURGEQ_ERR5_EID          144
#define CF_PURGEQ_ERR6_EID          145
#define CF_PURGEQ1_EID              146
#define CF_PURGEQ2_EID              147
#define CF_WRACT_ERR1_EID           148
#define CF_WRACT_ERR2_EID           149
#define CF_WRACT_TRANS_EID          150
#define CF_TBL_LD_ATTEMPT_EID       151
#define CF_OUT_SND_ERR1_EID         152
#define CF_OUT_SND_ERR2_EID         153
#define CF_OUT_SND_ERR3_EID         154
#define CF_QDIR_ACTIVEFILE_EID      155
#define CF_QDIR_OPENFILE_EID        156
#define CF_INV_FILENAME_EID         157
#define CF_GIVETAKE_CMD_EID         158
#define CF_QUICK_ERR1_EID           159
#define CF_QUICK_CMD_EID            160


#endif /* _cf_events_h_ */

/************************/
/*  End of File Comment */
/************************/
