/************************************************************************
** File:
**   $Id: ds_msgids.h 1.5.1.1 2015/02/28 17:13:45EST sstrege Exp  $
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
** Purpose: 
**  The CFS Data Storage (DS) Application Message IDs header file
**
** Notes:
**
** $Log: ds_msgids.h  $
** Revision 1.5.1.1 2015/02/28 17:13:45EST sstrege 
** Added copyright information
** Revision 1.5 2011/05/13 10:56:59EDT lwalling 
** Add definition for DS file info telemetry packet ID
** Revision 1.4 2009/08/31 16:47:47EDT lwalling 
** Remove references to DS_1HZ_MID and process file age tests during housekeeping request
** Revision 1.3 2009/07/20 13:53:09EDT lwalling 
** Update message ID's per version 0.21 of CFS Development Standards doc.
** Revision 1.2 2009/05/26 13:46:14EDT lwalling 
** Initial version of public header files
** Revision 1.1 2008/11/25 11:35:04EST rmcgraw 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/ds/fsw/platform_inc/project.pj
**
****************************************************************************/
#ifndef _ds_msgids_h_
#define _ds_msgids_h_

/**************************
** DS Command Message IDs
***********************/

#define DS_CMD_MID         0x18BB /**< \brief DS Ground Commands Message ID */
#define DS_SEND_HK_MID     0x18BC /**< \brief DS Send Hk Data Cmd Message ID*/


/***************************************
** DS Telemetry Message IDs
***************************************/

#define DS_HK_TLM_MID      0x08B8 /**< \brief DS Hk Telemetry Message ID ****/
#define DS_DIAG_TLM_MID    0x08B9 /**< \brief DS File Info Telemetry Message ID ****/


#endif /* _ds_msgids_h_ */

/************************/
/*  End of File Comment */
/************************/
