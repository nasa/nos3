/*
** File: utf_cfe_sb.c
** $Id: utf_cfe_sb.c 1.4 2012/01/13 12:51:59GMT-05:00 acudmore Exp  $
**
**      Copyright (c) 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement.
**  
**
** Purpose: This file contains the UTF Software Bus simulator support functions.
**
** References:
**
** Assumptions and Notes:
**
** $Date: 2012/01/13 12:51:59GMT-05:00 $
** $Revision: 1.4 $
** $Log: utf_cfe_sb.c  $
** Revision 1.4 2012/01/13 12:51:59GMT-05:00 acudmore 
** Changed license text to reflect open source
** Revision 1.3 2010/10/25 15:06:38EDT jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.2 2010/10/04 14:57:28EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.1 2008/04/17 08:09:37EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/src/project.pj
** Revision 1.3 2006/06/21 15:37:51EDT wfmoleski 
** Added copyright prologs to utf_sb files
** Revision 1.2 2006/03/24 12:39:11EST sslegel 
** Removed C++ comment
** Revision 1.1 2006/03/21 16:44:27EST sslegel
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/src/project.pj
** Revision 1.1 2004/12/21 15:00:55EST sslegel
** Initial revision
** Member added to project /home/mksdata/MKS-SDO-REPOSITORY/fsw/tools/utf/project.pj
** Revision 1.4  2004/12/03 16:24:19  sslegel
** Added new function UTF_set_sb_receive_return_status to change the default return status from a software bus receive call.
** Revision 1.3  2004/05/07 19:27:56  sslegel
** Fix revision history tag
**
*/

/*
** Include Section
*/
#include "cfe_sb.h"
#include "osapi.h"
#include "ccsds.h"

#include "utf_cfe_sb.h"
#include "utf_custom.h"
#include "utf_types.h"

/*
** Local Data Section
*/
#define SID_COUNT 0x2000                                        /* used to size arrays indexed by stream id */

static utf_packet_handler custom_packet_handlers[SID_COUNT];    /* indexed by stream_id */
static utf_packet_handler default_packet_handler = NULL;
static boolean            packet_output_header_only[SID_COUNT] = {FALSE};
static boolean            packet_output_disabled[SID_COUNT] = {FALSE};

/*
** Function definitions
*/

/******************************************************************************
**  Function:  UTF_set_header_only_packet_output()
**
**  Purpose:
**    Specify whether only the header of a packet is to be displayed for
**    a given stream ID.
*/
void UTF_set_header_only_packet_output (unsigned short sid, int header_only)
{
   packet_output_header_only[sid] = header_only;
}

/******************************************************************************
**  Function:  UTF_set_header_only_packet_output_all()
**
**  Purpose:
**    Specify whether only the header of a packet is to be displayed for
**    all stream IDs.
*/
void UTF_set_header_only_packet_output_all (int header_only)
{
   int i;

   for (i = 0; i < SID_COUNT; i++)
      packet_output_header_only[i] = header_only;
}

/******************************************************************************
**  Function:  UTF_disable_packet_output()
**
**  Purpose:
**    Specify that packets with a given stream ID should not be displayed.
*/
void UTF_disable_packet_output (unsigned short int packet_id)
{
   packet_output_disabled[packet_id] = TRUE;
}

/******************************************************************************
**  Function:  UTF_set_packet_handler()
**
**  Purpose:
**    Specify a customized displaying function for packets with a given
**    stream ID.
*/
void UTF_set_packet_handler (uint16 sid, utf_packet_handler handler)
{
   custom_packet_handlers[sid] = handler;
}

/******************************************************************************
**  Function:  UTF_set_default_packet_handler()
**
**  Purpose:
**    Specify a customized displaying function for packets that do not have
**    a specific displaying function.
*/
void UTF_set_default_packet_handler (utf_packet_handler handler)
{
   default_packet_handler = handler;
}

/******************************************************************************
**  Function:  UTF_put_packet_header()
**
**  Purpose:
**    Display the CCSDS header of a packet.
*/
void UTF_put_packet_header (uint8 source, CCSDS_PriHdr_t *packet)
{

    uint16              sid;
    CFE_TIME_SysTime_t  TimeFromMsg;

    sid = CCSDS_RD_SID(*packet);

    if (CCSDS_SID_TYPE(sid) == CCSDS_CMD)
	{

        if (CCSDS_SID_SHDR(sid) == CCSDS_HAS_SEC_HDR)
		{

            UTF_put_text("%s CMD: APID:%04x SEQ:%d LEN:%d FC:%d ",
                (source == UTF_SOURCE_SEND) ? ">": "<",
                CCSDS_RD_APID(*packet),
                CCSDS_RD_SEQ(*packet),
                CCSDS_RD_LEN(*packet),
                CCSDS_RD_FC(((CCSDS_CmdPkt_t *)packet)->SecHdr));
        }
        else
		{

            UTF_put_text("%s CMD: APID:%04x SEQ:%d LEN:%d ",
                (source == UTF_SOURCE_SEND) ? ">": "<",
                CCSDS_RD_APID(*packet),
                CCSDS_RD_SEQ(*packet),
                CCSDS_RD_LEN(*packet));
        }
    }
    else
	{

        if (CCSDS_SID_SHDR(sid) == CCSDS_HAS_SEC_HDR)
		{
            TimeFromMsg = CFE_SB_GetMsgTime((CFE_SB_MsgPtr_t)packet);
            UTF_put_text("%s TLM: APID:%04x SEQ:%d LEN:%d TIME:%08x %08x ",
                (source == UTF_SOURCE_SEND) ? ">": "<",
                CCSDS_RD_APID(*packet),
                CCSDS_RD_SEQ(*packet),
                CCSDS_RD_LEN(*packet),
                TimeFromMsg.Seconds,
                TimeFromMsg.Subseconds);
        }
        else
		{

            UTF_put_text("%s TLM: APID:%04x SEQ:%d LEN:%d ",
                (source == UTF_SOURCE_SEND) ? ">": "<",
                CCSDS_RD_APID(*packet),
                CCSDS_RD_SEQ(*packet),
                CCSDS_RD_LEN(*packet));
        }
    }
}

/******************************************************************************
**  Function:  UTF_put_packet()
**
**  Purpose:
**    Display a packet.
*/
void UTF_put_packet (uint8 source, CCSDS_PriHdr_t *packet)
{
    if (packet_output_disabled[CCSDS_RD_SID(*packet)])
        return;

    if (custom_packet_handlers[CCSDS_RD_SID(*packet)])
        custom_packet_handlers[CCSDS_RD_SID(*packet)](source, packet);
    else if (default_packet_handler) {
        (*default_packet_handler)(source, packet);
    }
    else {

        UTF_put_packet_header (source, packet);
        if (!packet_output_header_only [CCSDS_RD_SID(*packet)])
        {
            UTF_put_text("PKT:");
            UTF_put_data(((void *)packet), (uint16)(CCSDS_RD_LEN(*packet)), UTF_AS_WORD);
        }
        else {
            UTF_put_text("\n");
        }
    }
}
