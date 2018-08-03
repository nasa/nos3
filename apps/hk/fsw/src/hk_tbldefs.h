/************************************************************************
** File:
**   $Id: hk_tbldefs.h 1.2 2015/03/04 14:58:32EST sstrege Exp  $
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
**  The CFS Housekeeping (HK) Application header file
**
** Notes:
**
** $Log: hk_tbldefs.h  $
** Revision 1.2 2015/03/04 14:58:32EST sstrege 
** Added copyright information
** Revision 1.1 2008/09/16 09:29:48EDT rjmcgraw 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hk/fsw/src/project.pj
**
*************************************************************************/
#ifndef _hk_tbldefs_h_
#define _hk_tbldefs_h_


/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"


/*************************************************************************
** Type definitions
**************************************************************************/

/**  \brief HK Copy Table Entry Format 
*/

typedef struct
{
    CFE_SB_MsgId_t      InputMid;    /**< \brief MsgId of the input packet */
    uint16              InputOffset; /**< \brief ByteOffset into the input pkt where copy will begin */
    CFE_SB_MsgId_t      OutputMid;   /**< \brief MsgId of the output packet */
    uint16              OutputOffset;/**< \brief ByteOffset into the output pkt where data will be placed */
    uint16              NumBytes;    /**< \brief Number of data bytes to copy from input to output pkt */
} hk_copy_table_entry_t;


/**  \brief HK Run-time Table Entry Format 
*/
typedef struct
{
    CFE_SB_MsgPtr_t     OutputPktAddr;     /**< \brief Addr of output packet */
    uint8               InputMidSubscribed;/**< \brief Indicates if input MID has been subscribed to */
    uint8               DataPresent;       /**< \brief Indicates if the data associated with the entry is present */
} hk_runtime_tbl_entry_t;



#endif      /* _hk_tbldefs_h_ */

/************************/
/*  End of File Comment */
/************************/
