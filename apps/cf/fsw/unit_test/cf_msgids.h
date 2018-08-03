/************************************************************************
** File:
**   $Id: cf_msgids.h 1.2 2015/03/06 14:47:52EST sstrege Exp  $
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
**  The CF Application Message IDs header file
**
** Notes:
**
** $Log: cf_msgids.h  $
** Revision 1.2 2015/03/06 14:47:52EST sstrege 
** Added copyright information
** Revision 1.1 2011/05/04 09:59:13EDT rmcgraw 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cf/fsw/unit_test/project.pj
** Revision 1.4 2010/10/20 11:06:35EDT rmcgraw 
** DCR12495:1 Removed unused MsgId CF_DIAG_TLM_MID
** Revision 1.3 2010/03/12 12:14:35EST rmcgraw 
** DCR11510:1 Initial check-in towards CF Version 1000
** Revision 1.2 2009/12/08 09:03:22EST rmcgraw 
** DCR10350:3 Replaced History Tlm with Config tlm
** Revision 1.1 2009/11/24 12:47:35EST rmcgraw 
** Initial revision
** Member added to CFS CF project
**
*************************************************************************/
#ifndef _cf_msgids_h_
#define _cf_msgids_h_

/**************************
** CF Command Message IDs
***************************/

#define CF_CMD_MID                      0x18B3
#define CF_SEND_HK_MID                  0x18B4 
#define CF_WAKE_UP_REQ_CMD_MID          0x18B5
#define CF_SPARE1_CMD_MID               0x18B6
#define CF_SPARE2_CMD_MID               0x18B7
#define CF_SPARE3_CMD_MID               0x18B8
#define CF_SPARE4_CMD_MID               0x18B9
#define CF_SPARE5_CMD_MID               0x18BA
#define CF_INCOMING_PDU_MID             0x1FFD 


/***************************
** CF Telemetry Message IDs
****************************/

#define CF_HK_TLM_MID                   0x08B0
#define CF_TRANS_TLM_MID                0x08B1
#define CF_CONFIG_TLM_MID               0x08B2
#define CF_SPARE0_TLM_MID               0x08B3
#define CF_SPARE1_TLM_MID               0x08B4
#define CF_SPARE2_TLM_MID               0x08B5
#define CF_SPARE3_TLM_MID               0x08B6
#define CF_SPARE4_TLM_MID               0x08B7

/* 
** NOTE: the definition below is NOT used by the code. The code uses the MsgId 
** defined in the CF table. For the purpose of keeping all CF related message
** IDs defined in this file, the CF table should reference this macro 
** definition.
*/
#define CF_SPACE_TO_GND_PDU_MID         0x0FFD

#endif /* _cf_msgids_h_ */

/************************/
/*  End of File Comment */
/************************/
