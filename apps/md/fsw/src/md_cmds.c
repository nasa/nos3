/*************************************************************************
** File:
**   $Id: md_cmds.c 1.9 2015/03/01 17:17:42EST sstrege Exp  $
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
**   Functions for processing individual CFS Memory Dwell commands
**
**   $Log: md_cmds.c  $
**   Revision 1.9 2015/03/01 17:17:42EST sstrege 
**   Added copyright information
**   Revision 1.8 2012/01/09 18:01:10EST aschoeni 
**   Added ability to not force 32 bit alignment
**   Revision 1.7 2009/09/30 17:31:07EDT aschoeni 
**   Added message if a dwell is jammed to zero in an enabled table
**   Revision 1.6 2009/09/30 15:53:32EDT aschoeni 
**   Updated Enable command to output event if table with a delay of 0 is enabled.
**   Revision 1.5 2009/09/30 14:13:40EDT aschoeni 
**   Added check to make sure signature is null terminated.
**   Revision 1.4 2009/01/12 14:33:23EST nschweis 
**   Removed debug statements from source code.  CPID 4688:1.
**   Revision 1.3 2008/12/10 14:59:57EST nschweis 
**   Modified to test changes in DCR #2624: Keep Memory Dwell in sync with Table Services buffer.
**   Modified code so that state changes triggered by Start, Stop, Jam, and Set Signature commands are
**   copied to the Table Services buffer.
**   CPID 2624:1.
**   Revision 1.2 2008/08/07 16:55:08EDT nsschweiss 
**   Changed name of included file from cfs_lib.h to cfs_utils.h.
**   Revision 1.1 2008/07/02 13:47:00EDT nsschweiss 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/md/fsw/src/project.pj
** 
*************************************************************************/

/*************************************************************************
** Includes
*************************************************************************/
#include "md_cmds.h"
#include "md_utils.h"
/* Need to include md_msg.h for command type definitions */
#include "md_msg.h"
#include "md_platform_cfg.h"
#include <string.h>
#include "md_app.h"
#include "md_events.h"
#include "cfs_utils.h"
#include "md_dwell_tbl.h"

/* Global Data */
extern MD_AppData_t MD_AppData;

/******************************************************************************/

void MD_ProcessStartCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    uint16              TableId;
    uint16              TableIndex;
    MD_CmdStartStop_t  *Start;
    boolean             AnyTablesInMask;
    
    AnyTablesInMask         = FALSE;
    
    Start = (MD_CmdStartStop_t *  ) MessagePtr;

    /*  Preview tables specified by command:                   */
    /*  Check that there's at least one valid table specified. */
    for (TableId=1; TableId <= MD_NUM_DWELL_TABLES; TableId++)
    {
        if (MD_TableIsInMask(TableId, Start->TableMask))
        {
            /* At least one valid Table Id is in Mask */
            AnyTablesInMask=TRUE;
        }
    }
    
    /* 
    ** Handle specified operation.
    ** If nominal, start each of the specified tables.
    ** If error case was encountered, issue error message. 
    */
    if (AnyTablesInMask)
    {  /* Handle Nominal Case */
            
        for (TableId=1; TableId <= MD_NUM_DWELL_TABLES; TableId++)
        {
           if (MD_TableIsInMask(TableId, Start->TableMask))
           {
              /* Setting Countdown to 1 causes a dwell packet to be issued */
              /* on first wakeup call received. */
              TableIndex = TableId-1;
              MD_AppData.MD_DwellTables[ TableIndex ].Enabled = MD_DWELL_STREAM_ENABLED;
              MD_AppData.MD_DwellTables[ TableIndex ].Countdown = 1;
              MD_AppData.MD_DwellTables[ TableIndex ].CurrentEntry = 0;
              MD_AppData.MD_DwellTables[ TableIndex ].PktOffset = 0;
                          
              /* Change value in Table Services managed buffer */
              MD_UpdateTableEnabledField (TableIndex, MD_DWELL_STREAM_ENABLED);

              /* If table contains a rate of zero, report that no processing will occur */
              if (MD_AppData.MD_DwellTables[ TableIndex ].Rate == 0)
              {
                  CFE_EVS_SendEvent(MD_ZERO_RATE_CMD_INF_EID, CFE_EVS_INFORMATION, 
                  "Dwell Table %d is enabled with a delay of zero so no processing will occur", TableId); 
              }       
           }
        }
        
        MD_AppData.CmdCounter++;

        CFE_EVS_SendEvent(MD_START_DWELL_INF_EID,  CFE_EVS_INFORMATION,
        "Start Dwell Table command processed successfully for table mask 0x%04X",
                                                      Start->TableMask);
    }
    else /* No valid table id's specified in mask */
    {
        MD_AppData.ErrCounter++;
        CFE_EVS_SendEvent(MD_EMPTY_TBLMASK_ERR_EID,  CFE_EVS_ERROR,
       "%s command rejected because no tables were specified in table mask (0x%04X)",
        "Start Dwell", Start->TableMask );
    }
    return;
    
}  /* End of MD_ProcessStartCmd */
 
/******************************************************************************/

void MD_ProcessStopCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    MD_CmdStartStop_t  *Stop;
    uint16              TableId;
    uint16              TableIndex;
    boolean             AnyTablesInMask;
    
    AnyTablesInMask   =  FALSE;
    Stop              = (MD_CmdStartStop_t *  ) MessagePtr;

    
    for (TableId=1; TableId <= MD_NUM_DWELL_TABLES; TableId++)
    {
        if (MD_TableIsInMask(TableId, Stop->TableMask))
        {
            TableIndex = TableId-1;
            MD_AppData.MD_DwellTables[ TableIndex ].Enabled = MD_DWELL_STREAM_DISABLED;
            MD_AppData.MD_DwellTables[ TableIndex ].Countdown = 0;
            MD_AppData.MD_DwellTables[ TableIndex ].CurrentEntry = 0;
            MD_AppData.MD_DwellTables[ TableIndex ].PktOffset = 0;
            
            AnyTablesInMask=TRUE;
            
            /* Change value in Table Services managed buffer */
            MD_UpdateTableEnabledField (TableIndex, MD_DWELL_STREAM_DISABLED);
            
        }
    }
    
    if (AnyTablesInMask)
    {
        CFE_EVS_SendEvent(MD_STOP_DWELL_INF_EID,  CFE_EVS_INFORMATION,
            "Stop Dwell Table command processed successfully for table mask 0x%04X",
             Stop->TableMask );

        MD_AppData.CmdCounter++;
    }
    else
    {
        CFE_EVS_SendEvent(MD_EMPTY_TBLMASK_ERR_EID,  CFE_EVS_ERROR,
       "%s command rejected because no tables were specified in table mask (0x%04X)",
        "Stop Dwell", Stop->TableMask );
        MD_AppData.ErrCounter++;
    }
    return;
}  /* End of MD_ProcessStopCmd */
 
/******************************************************************************/

void MD_ProcessJamCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    /* Local variables */
    MD_CmdJam_t            *Jam = 0;
    boolean                 AllInputsValid = TRUE;
    uint32                  ResolvedAddr=0;
    MD_DwellControlEntry_t *DwellEntryPtr = 0; /* points to local task data */
    uint16                  EntryIndex = 0;
    uint8                   TableIndex = 0;
    CFS_SymAddr_t           NewDwellAddress;
            
    /*
    **  Cast message to Jam Command.
    */
    Jam         = (MD_CmdJam_t * ) MessagePtr;
    
    /* In case Dwell Address sym name isn't null terminated, do it now. */
    Jam->DwellAddress.SymName[OS_MAX_SYM_LEN - 1] = '\0';
    
    /*
    **  Check that TableId and EntryId command arguments,
    **  which are used as array indexes, are valid.
    */
    if (  !MD_ValidTableId ( Jam->TableId) )
    {        
        CFE_EVS_SendEvent(MD_INVALID_JAM_TABLE_ERR_EID, CFE_EVS_ERROR,
         "Jam Cmd rejected due to invalid Tbl Id arg = %d (Expect 1.. %d)",
          Jam->TableId, MD_NUM_DWELL_TABLES);

        AllInputsValid = FALSE;
    }
    else if ( !MD_ValidEntryId ( Jam->EntryId))
    {        
        CFE_EVS_SendEvent(MD_INVALID_ENTRY_ARG_ERR_EID, CFE_EVS_ERROR,
         "Jam Cmd rejected due to invalid Entry Id arg = %d (Expect 1.. %d)",
          Jam->EntryId, MD_DWELL_TABLE_SIZE);

        AllInputsValid = FALSE;
    }
    
    /*
    **  If all inputs checked so far are valid, continue.
    */    
    if (AllInputsValid == TRUE)
    {
        TableIndex  = Jam->TableId-1;
        EntryIndex  = Jam->EntryId-1;
                
        DwellEntryPtr   = (MD_DwellControlEntry_t *) &MD_AppData.MD_DwellTables[TableIndex].Entry[EntryIndex];

        if (Jam->FieldLength == 0)
        /*
        **  Jam a null entry.  Set all entry fields to zero.
        */    
        {
            /* Assign local values */
            DwellEntryPtr->ResolvedAddress = 0;
            DwellEntryPtr->Length          = 0;
            DwellEntryPtr->Delay           = 0;
            
            /* Update Table Services buffer */
            NewDwellAddress.Offset = 0;
            NewDwellAddress.SymName[0] = '\0';
            MD_UpdateTableDwellEntry (TableIndex, EntryIndex, 0, 0, NewDwellAddress);

            /* Issue event */
            CFE_EVS_SendEvent(MD_JAM_NULL_DWELL_INF_EID, CFE_EVS_INFORMATION,
            "Successful Jam of a Null Dwell Entry to Dwell Tbl#%d Entry #%d", 
                           Jam->TableId, Jam->EntryId  );
        } 
        else
        /*
        **  Process non-null entry.
        */   
        { 
            /*
            **  Check that address and field length arguments pass all validity checks.
            */    

            /* Resolve and Validate Dwell Address */
            if (CFS_ResolveSymAddr(&Jam->DwellAddress,&ResolvedAddr) == FALSE)
            {
                /* If DwellAddress argument couldn't be resolved, issue error event */
                CFE_EVS_SendEvent(MD_CANT_RESOLVE_JAM_ADDR_ERR_EID, 
                                  CFE_EVS_ERROR,
                                 "Jam Cmd rejected because symbolic address '%s' couldn't be resolved",
                                  Jam->DwellAddress.SymName);
                AllInputsValid = FALSE;
            }
            else if (!MD_ValidFieldLength(Jam->FieldLength))
            {        
                CFE_EVS_SendEvent(MD_INVALID_LEN_ARG_ERR_EID, CFE_EVS_ERROR,
                                 "Jam Cmd rejected due to invalid Field Length arg = %d (Expect 0,1,2,or 4)",
                                  Jam->FieldLength);
                AllInputsValid = FALSE;
            }
            else if (!MD_ValidAddrRange(ResolvedAddr, Jam->FieldLength))
            {
                /* Issue event message that ResolvedAddr is invalid */
                CFE_EVS_SendEvent(MD_INVALID_JAM_ADDR_ERR_EID, CFE_EVS_ERROR,
                                 "Jam Cmd rejected because address 0x%08X is not in a valid range", 
                                  ResolvedAddr);
                AllInputsValid = FALSE;
            }
#if MD_ENFORCE_DWORD_ALIGN == 0
            else  if ((Jam->FieldLength == 4) && 
                      CFS_Verify16Aligned(ResolvedAddr, (uint32)Jam->FieldLength) != TRUE)
            {
                CFE_EVS_SendEvent(MD_JAM_ADDR_NOT_16BIT_ERR_EID, CFE_EVS_ERROR,
                                 "Jam Cmd rejected because address 0x%08X is not 16-bit aligned", 
                                  ResolvedAddr);
                AllInputsValid = FALSE;
            }
#else    
            else  if ((Jam->FieldLength == 4) && 
                      CFS_Verify32Aligned(ResolvedAddr, (uint32)Jam->FieldLength) != TRUE)
            {
                CFE_EVS_SendEvent(MD_JAM_ADDR_NOT_32BIT_ERR_EID, CFE_EVS_ERROR,
                                 "Jam Cmd rejected because address 0x%08X is not 32-bit aligned", 
                                  ResolvedAddr);
                AllInputsValid = FALSE;
            }
#endif
            else  if ((Jam->FieldLength == 2) && CFS_Verify16Aligned(ResolvedAddr, (uint32)Jam->FieldLength) != TRUE)
            {
                CFE_EVS_SendEvent(MD_JAM_ADDR_NOT_16BIT_ERR_EID, CFE_EVS_ERROR,
                                 "Jam Cmd rejected because address 0x%08X is not 16-bit aligned", 
                                  ResolvedAddr);
                AllInputsValid = FALSE;
            }


            if (AllInputsValid == TRUE)
            /* 
            ** Perform Jam Operation : Copy Resolved DwellAddress, Length, and Delay to
            ** local control structure.
            */
            {
                /* Jam the new values into Application control structure */
                DwellEntryPtr->ResolvedAddress = ResolvedAddr;
                DwellEntryPtr->Length          = Jam->FieldLength;
                DwellEntryPtr->Delay           = Jam->DwellDelay;
                
                /* Update values in Table Services buffer */
                NewDwellAddress.Offset = Jam->DwellAddress.Offset;
                
                strncpy (NewDwellAddress.SymName, Jam->DwellAddress.SymName, OS_MAX_SYM_LEN);
                
                MD_UpdateTableDwellEntry (TableIndex, EntryIndex, Jam->FieldLength, Jam->DwellDelay, NewDwellAddress);
            
                /* Issue event */
                CFE_EVS_SendEvent(MD_JAM_DWELL_INF_EID, CFE_EVS_INFORMATION,
                          "Successful Jam to Dwell Tbl#%d Entry #%d", 
                           Jam->TableId, Jam->EntryId  );
            }
            
        } /* end else Process non-null entry */  
          
    } /* end if AllInputsValid */    
    
    /*
    **  Handle bookkeeping.
    */
    if (AllInputsValid == TRUE)
    {
        MD_AppData.CmdCounter++;   
                    
        /* Update Dwell Table Control Info, including rate */
        MD_UpdateDwellControlInfo(TableIndex);

        /* If table contains a rate of zero, and it enabled report that no processing will occur */
        if ((MD_AppData.MD_DwellTables[ TableIndex ].Rate == 0) &&
            (MD_AppData.MD_DwellTables[ TableIndex ].Enabled == MD_DWELL_STREAM_ENABLED))
        {
            CFE_EVS_SendEvent(MD_ZERO_RATE_CMD_INF_EID, CFE_EVS_INFORMATION, 
            "Dwell Table %d is enabled with a delay of zero so no processing will occur", Jam->TableId); 
        }
    }
    else
    {
        MD_AppData.ErrCounter++;
    }
            
    return;
    
} /* End of MD_ProcessJamCmd */



/******************************************************************************/
#if MD_SIGNATURE_OPTION == 1   

void MD_ProcessSignatureCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    MD_CmdSetSignature_t  *SignatureCmd;
    uint16                 TblId;
    uint16                 StringLength;
    
    /*
    **  Cast message to Set Signature Command.
    */
    SignatureCmd         = (MD_CmdSetSignature_t * ) MessagePtr;

    TblId = SignatureCmd->TableId;

    /*
    **  Check for Null Termination of string
    */
    for(StringLength = 0; StringLength < MD_SIGNATURE_FIELD_LENGTH; StringLength++)
    {
       if(SignatureCmd->Signature[StringLength] == '\0')
          break;
    }

    if (StringLength >= MD_SIGNATURE_FIELD_LENGTH)
    {      
        CFE_EVS_SendEvent(MD_INVALID_SIGNATURE_LENGTH_ERR_EID, CFE_EVS_ERROR,
         "Set Signature cmd rejected due to invalid Signature length");
          
        MD_AppData.ErrCounter++;
    }

    /*
    ** Check for valid TableId argument
    */
    else if (  !MD_ValidTableId ( TblId) )
    {      
        CFE_EVS_SendEvent(MD_INVALID_SIGNATURE_TABLE_ERR_EID, CFE_EVS_ERROR,
         "Set Signature cmd rejected due to invalid Tbl Id arg = %d (Expect 1.. %d)",
          TblId, MD_NUM_DWELL_TABLES);
          
        MD_AppData.ErrCounter++;
    }

    else
    
    /*
    **  Handle nominal case.
    */
    {
       /* Copy signature field to local dwell control structure */
       strncpy(MD_AppData.MD_DwellTables[TblId-1].Signature, 
             SignatureCmd->Signature, MD_SIGNATURE_FIELD_LENGTH);

       /* Update signature in Table Services buffer */
       MD_UpdateTableSignature(TblId-1,SignatureCmd->Signature);
       
       CFE_EVS_SendEvent(MD_SET_SIGNATURE_INF_EID, CFE_EVS_INFORMATION,
                          "Successfully set signature for Dwell Tbl#%d to '%s'", 
                           TblId, SignatureCmd->Signature  );

       MD_AppData.CmdCounter++;
    }
    return;

}

#endif

/************************/
/*  End of File Comment */
/************************/
