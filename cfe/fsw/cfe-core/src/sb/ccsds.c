/******************************************************************************
** File:  ccsds.c
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
**
** Purpose:
**      Functions for working with CCSDS headers.
**
** $Log: ccsds.c  $
** Revision 1.5 2012/01/13 12:15:11GMT-05:00 acudmore 
** Changed license text to reflect open source
** Revision 1.4 2010/10/25 15:00:51EDT jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.3 2010/10/04 15:21:32EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.2 2009/06/10 09:17:28EDT acudmore 
** Updated OS_Mem* and OS_BSP* to CFE_PSP_*
** Revision 1.1 2008/04/17 08:05:29EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/sb/project.pj
** Revision 1.3 2007/09/24 11:02:22EDT rjmcgraw 
** Removed unused macros
** Revision 1.2 2006/06/12 15:26:03EDT rjmcgraw 
** Added legal statement
**
******************************************************************************/

/*
** Include Files
*/

#include "common_types.h"
#include "ccsds.h"
#include "osapi.h"
#include "cfe_psp.h"


/******************************************************************************
**  Function:  CCSDS_InitPkt()
**
**  Purpose:
**    Initialize a CCSDS packet.  The primary header is initialized with
**    specified values, and if the Clear flag is set, the rest of the packet
**    is filled with zeros.
**
**  Arguments:
**    PktPtr   : Pointer to primary header of packet.
**    StreamId : Stream ID to use for the packet.
**    Length   : Length of the packet in bytes.
**    Clear    : Indicates whether to clear the entire packet:
**                TRUE = fill sequence count and packet data with zeros
**                       (used after a cold restart)
**                FALSE = leave sequence count and packet data unchanged
**                       (used after a warm restart if data must be preserved)
**
**  Return:
**    (none)
*/

void CCSDS_InitPkt (CCSDS_PriHdr_t  *PktPtr,
                    uint16           StreamId,
                    uint16           Length,
                    boolean          Clear )
{
   uint16     SeqCount;

   /* Save the sequence count in case it must be preserved. */
   SeqCount = CCSDS_RD_SEQ(*PktPtr);

   /* Zero the entire packet if needed. */
   if (Clear)  CFE_PSP_MemSet((void *)PktPtr, 0, Length);

   /* Clear the primary header. */
   CCSDS_CLR_PRI_HDR(*PktPtr);

   /* Set the stream ID and length fields in the primary header. */
   CCSDS_WR_SID(*PktPtr, StreamId);
   CCSDS_WR_LEN(*PktPtr, Length);

   /* Restore the sequence count if needed. */
   if (!Clear)  CCSDS_WR_SEQ(*PktPtr, SeqCount);

} /* END CCSDS_InitPkt() */


/******************************************************************************
**  Function:  CCSDS_LoadCheckSum()
**
**  Purpose:
**    Compute and load a checksum for a CCSDS command packet that has a
**    secondary header.
**
**  Arguments:
**    PktPtr   : Pointer to header of command packet.  The packet must
**               have a secondary header and the length in the primary
**               header must be correct.  The checksum field in the packet
**               will be modified.
**
**  Return:
**    (none)
*/

void CCSDS_LoadCheckSum (CCSDS_CmdPkt_t *PktPtr)
{
   uint8    CheckSum;

   /* Clear the checksum field so the new checksum is correct. */
   CCSDS_WR_CHECKSUM(PktPtr->SecHdr, 0);

   /* Compute and load new checksum. */
   CheckSum = CCSDS_ComputeCheckSum(PktPtr);
   CCSDS_WR_CHECKSUM(PktPtr->SecHdr, CheckSum);

} /* END CCSDS_LoadCheckSum() */


/******************************************************************************
**  Function:  CCSDS_ValidCheckSum()
**
**  Purpose:
**    Determine whether a checksum in a command packet is valid.
**
**  Arguments:
**    PktPtr   : Pointer to header of command packet.  The packet must
**               have a secondary header and the length in the primary
**               header must be correct.
**
**  Return:
**    TRUE if checksum of packet is valid; FALSE if not.
**    A valid checksum is 0.
*/

boolean CCSDS_ValidCheckSum (CCSDS_CmdPkt_t *PktPtr)
{

   return (CCSDS_ComputeCheckSum(PktPtr) == 0);

} /* END CCSDS_ValidCheckSum() */


/******************************************************************************
**  Function:  CCSDS_ComputeCheckSum()
**
**  Purpose:
**    Compute the checksum for a command packet.  The checksum is the XOR of
**    all bytes in the packet; a valid checksum is zero.
**
**  Arguments:
**    PktPtr   : Pointer to header of command packet.  The packet must
**               have a secondary header and the length in the primary
**               header must be correct.
**
**  Return:
**    TRUE if checksum of packet is valid; FALSE if not.
*/

uint8 CCSDS_ComputeCheckSum (CCSDS_CmdPkt_t *PktPtr)
{
   uint16   PktLen   = CCSDS_RD_LEN(PktPtr->PriHdr);
   uint8   *BytePtr  = (uint8 *)PktPtr;
   uint8    CheckSum;

   CheckSum = 0xFF;
   while (PktLen--)  CheckSum ^= *(BytePtr++);

   return CheckSum;

} /* END CCSDS_ComputeCheckSum() */

/*****************************************************************************/
