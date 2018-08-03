/************************************************************************
** File:
**   $Id: md_dwell_pkt.c 1.6 2015/03/01 17:17:51EST sstrege Exp  $
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
**   Functions used to populate and send Memory Dwell packets.
**
**   $Log: md_dwell_pkt.c  $
**   Revision 1.6 2015/03/01 17:17:51EST sstrege 
**   Added copyright information
**   Revision 1.5 2009/06/12 14:19:06EDT rmcgraw 
**   DCR82191:1 Changed OS_Mem function calls to CFE_PSP_Mem
**   Revision 1.4 2009/01/12 14:33:27EST nschweis 
**   Removed debug statements from source code.  CPID 4688:1.
**   Revision 1.3 2008/10/21 13:59:02EDT nsschweiss 
**   Added MD_StartDwellStream to initialize dwell packet processing parameters.
**   Revision 1.2 2008/08/08 13:38:08EDT nsschweiss 
**   1) Changed name of include file from cfs_lib.h to cfs_utils.h.
**   2) Changed the way the length of the dwell packet is computed.
**   Revision 1.1 2008/07/02 13:48:22EDT nsschweiss 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/md/fsw/src/project.pj
** 
*************************************************************************/

/*************************************************************************
** Includes
*************************************************************************/
#include "md_dwell_pkt.h"
#include "md_utils.h"
#include "md_app.h"
#include "cfs_utils.h"
#include <string.h>

extern MD_AppData_t MD_AppData;


/******************************************************************************/

void MD_DwellLoop( void )
{
    uint16                    TblIndex;
    uint16                    EntryIndex;
    uint16                    NumDwellAddresses;
    MD_DwellPacketControl_t  *TblPtr;
    
    /* Check each dwell table */
    for (TblIndex = 0; TblIndex < MD_NUM_DWELL_TABLES ; TblIndex++)
    {
    
        TblPtr = &MD_AppData.MD_DwellTables[TblIndex];
        NumDwellAddresses = TblPtr->AddrCount;
        
        /* Process enabled dwell tables */
        if ((TblPtr->Enabled == MD_DWELL_STREAM_ENABLED) &&  (TblPtr->Rate > 0))
        {
            
            /*
            ** Handle special case that dwell pkt is already full because
            ** pkt size was shortened after data had been written to the pkt.
            */
            
            if (  TblPtr->CurrentEntry >= NumDwellAddresses)
            {
                
                MD_SendDwellPkt( TblIndex );
                                    
                /* Initialize CurrentEntry index */
                TblPtr->CurrentEntry = 0;
                TblPtr->PktOffset = 0;
                TblPtr->Countdown = TblPtr->Entry[NumDwellAddresses - 1 ].Delay;

            }
            
            else
            /*
            ** Handle nominal processing
            */
            {
                /* Decrement counter */
                TblPtr->Countdown--;

            
                /* Check if it's time to collect data */
                while (TblPtr->Countdown == 0) 
                {
                    EntryIndex = TblPtr->CurrentEntry;
                
                    /* Read data for next address and write it to dwell pkt */
                    MD_GetDwellData(TblIndex, EntryIndex);
                
                    /* Check if the dwell pkt is now full */
                    if (EntryIndex == NumDwellAddresses - 1) 

                    /* Case:  Just filled last active entry of dwell table */
                    {
                         
                        /* 
                        ** Send dwell packet 
                        */
                        
                        MD_SendDwellPkt( TblIndex );
                        
                        /*
                        ** Assign control values to cause dwell processing to 
                        ** continue at beginning of dwell control structure.
                        */
                    
                        /* Reset countdown timer based on current Delay field */
                        TblPtr->Countdown = TblPtr->Entry[EntryIndex ].Delay;
                                
                        /* Initialize CurrentEntry index */
                        TblPtr->CurrentEntry = 0;
                        TblPtr->PktOffset = 0;
                    }
                    
                    else 
                    /* Case: There are more addresses to read for current pkt.*/
                    {
                        /*
                        ** Assign control values to cause dwell processing to 
                        ** continue at next entry in dwell control structure.
                        */

                        /* Reset countdown timer based on current Delay field */
                        TblPtr->Countdown = TblPtr->Entry[EntryIndex ].Delay;
                        
                        /* Increment CurrentEntry index */
                        TblPtr->CurrentEntry++;
                    }

                } /* end while Countdown == 0 */
                
            } /* end else handle nominal processing */
            
        } /* end if current dwell stream enabled */
        
    } /* end for each dwell table */
    
} /* End of MD_DwellLoop */

/******************************************************************************/

int32 MD_GetDwellData( uint16 TblIndex, uint16 EntryIndex )
{
    uint8                    NumBytes;  /* Num of bytes to read */
    uint32                   MemReadVal; /* 1-, 2-, or 4-byte value */
    MD_DwellPacketControl_t *TblPtr; /* Points to table struct */
    uint32                   DwellAddress;    /* dwell address */
    int32                    Status;
    
    Status  = CFE_SUCCESS;
    
    /* Initialize pointer to current table */
    TblPtr = (MD_DwellPacketControl_t *)&MD_AppData.MD_DwellTables[TblIndex];
    
    /* How many bytes to read?*/
    NumBytes = TblPtr->Entry[EntryIndex].Length;
    
    /* fetch data pointed to by this address */
    DwellAddress = TblPtr->Entry[EntryIndex].ResolvedAddress;
    
    if (NumBytes == 1)
    {
       if (CFE_PSP_MemRead8( DwellAddress, (uint8 *) &MemReadVal ) != CFE_SUCCESS)
       {
          Status = -1;
       }
    }
    
    else if (NumBytes == 2)
    {
       if (CFE_PSP_MemRead16( DwellAddress, (uint16 *) &MemReadVal ) != CFE_SUCCESS)
       {
          Status = -1;
       }
    }
    
    else if (NumBytes == 4)
    {
       if (CFE_PSP_MemRead32( DwellAddress, &MemReadVal ) != CFE_SUCCESS)
       {
          Status = -1;
       }
    }
    else /* Invalid dwell length */
         /* Shouldn't ever get here unless length value was corrupted. */
    {
       Status = -1;
    }
    
    
    /* If value was read successfully, copy value to dwell packet. */ 
    /* Wouldn't want to copy, if say, there was an invalid length & we */
    /* didn't read. */
    if (Status == CFE_SUCCESS) 
    {  
       CFE_PSP_MemCpy( (void*) &MD_AppData.MD_DwellPkt[TblIndex].Data[TblPtr->PktOffset],
        (void*) &MemReadVal,
        NumBytes);
    }
        
    /* Update write location in dwell packet */
    TblPtr->PktOffset += NumBytes;
    
    return Status;
    
} /* End of MD_GetDwellData */


/******************************************************************************/

void MD_SendDwellPkt( uint16 TableIndex )
{
    uint16 DwellPktSize;        /* Dwell Packet Size, in bytes */
    
    /* Assign pointers to structures */
    MD_DwellPacketControl_t *TblPtr = &MD_AppData.MD_DwellTables[TableIndex]; 
    MD_DwellPkt_t           *PktPtr = &MD_AppData.MD_DwellPkt[TableIndex]; 

    /*
    ** Assign packet fields.
    */
    PktPtr->TableId   = TableIndex + 1;
    PktPtr->AddrCount = TblPtr->AddrCount;
    PktPtr->Rate      = TblPtr->Rate;
#if MD_SIGNATURE_OPTION == 1   
    strncpy(PktPtr->Signature, TblPtr->Signature, MD_SIGNATURE_FIELD_LENGTH - 1);
    /* Make sure string is null-terminated. */
    PktPtr->Signature[MD_SIGNATURE_FIELD_LENGTH - 1] = '\0';
#endif
    PktPtr->ByteCount = TblPtr->DataSize;

    /*
    ** Set packet length in header.
    */

    DwellPktSize = MD_DWELL_PKT_LNGTH - MD_DWELL_TABLE_SIZE * 4 + TblPtr->DataSize;
        

    CFE_SB_SetTotalMsgLength((CFE_SB_Msg_t *)PktPtr, DwellPktSize);
    
    /*
    ** Send housekeeping telemetry packet.
    */
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) PktPtr);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) PktPtr);
    
} /* End of MD_SendDwellPkt */

/******************************************************************************/

void MD_StartDwellStream (uint16 TableIndex )
{
    MD_AppData.MD_DwellTables[ TableIndex ].Countdown = 1;
    MD_AppData.MD_DwellTables[ TableIndex ].CurrentEntry = 0;
    MD_AppData.MD_DwellTables[ TableIndex ].PktOffset = 0;

} /* End of MD_StartDwellStream */

/************************/
/*  End of File Comment */
/************************/
