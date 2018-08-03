/************************************************************************
 ** File:
 **   $Id: cs_utils.c 1.4 2017/02/16 15:33:14EST mdeschu Exp  $
 **
 **   Copyright (c) 2007-2014 United States Government as represented by the 
 **   Administrator of the National Aeronautics and Space Administration. 
 **   All Other Rights Reserved.  
 **
 **   This software was created at NASA's Goddard Space Flight Center.
 **   This software is governed by the NASA Open Source Agreement and may be 
 **   used, distributed and modified only pursuant to the terms of that 
 **   agreement.
 **
 ** Purpose: 
 **   The CFS Checksum (CS) Application's utility functions
 **
 *************************************************************************/

/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/
#include "cfe.h"
#include "cs_app.h"
#include "cs_events.h"
#include "cs_compute.h"
#include "cs_utils.h"
#include <string.h>

/**************************************************************************
 **
 ** Functions
 **
 **************************************************************************/
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Zero out the temp chcksum values of Eeprom                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ZeroEepromTempValues(void)
{
    uint16                                  Loop;
    
    for ( Loop = 0; Loop < CS_MAX_NUM_EEPROM_TABLE_ENTRIES; Loop++)
    {
        CS_AppData.ResEepromTblPtr[Loop].TempChecksumValue = 0;
        CS_AppData.ResEepromTblPtr[Loop].ByteOffset = 0;
    }
    return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Zero out the temp chcksum values of Memory                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ZeroMemoryTempValues(void)
{
    uint16                                  Loop;
    
    for ( Loop = 0; Loop < CS_MAX_NUM_MEMORY_TABLE_ENTRIES; Loop++)
    {
        CS_AppData.ResMemoryTblPtr[Loop].TempChecksumValue = 0;
        CS_AppData.ResMemoryTblPtr[Loop].ByteOffset = 0;
    }
    
    return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Zero out the temp chcksum values of Tables                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ZeroTablesTempValues(void)
{
    uint16                                  Loop;
    
    for ( Loop = 0; Loop < CS_MAX_NUM_TABLES_TABLE_ENTRIES; Loop++)
    {
        CS_AppData.ResTablesTblPtr[Loop].TempChecksumValue = 0;
        CS_AppData.ResTablesTblPtr[Loop].ByteOffset = 0;
    }
    
    return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Zero out the temp chcksum values of Applications             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ZeroAppTempValues(void)
{
    uint16                                  Loop;
    
    for ( Loop = 0; Loop < CS_MAX_NUM_APP_TABLE_ENTRIES; Loop ++)
    {
        CS_AppData.ResAppTblPtr[Loop].TempChecksumValue = 0;
        CS_AppData.ResAppTblPtr[Loop].ByteOffset = 0;
    }
    
    return;
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Zero out the temp chcksum values ofthe cFE core              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ZeroCfeCoreTempValues(void)
{    
    CS_AppData.CfeCoreCodeSeg.TempChecksumValue = 0;
    CS_AppData.CfeCoreCodeSeg.ByteOffset = 0;    
    
    return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Zero out the temp chcksum values of the OS code segment      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ZeroOSTempValues(void)
{    
    CS_AppData.OSCodeSeg.TempChecksumValue = 0;
    CS_AppData.OSCodeSeg.ByteOffset = 0;    
    
    return;
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Nullifies the entries in the Results table for All           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_InitializeDefaultTables(void)
{
    uint16                                  Loop;

    for (Loop = 0; Loop < CS_MAX_NUM_EEPROM_TABLE_ENTRIES; Loop++)
    {                
        CS_AppData.DefaultEepromDefTable[Loop].State              = CS_STATE_EMPTY;
        CS_AppData.DefaultEepromDefTable[Loop].NumBytesToChecksum = 0;
        CS_AppData.DefaultEepromDefTable[Loop].StartAddress       =0;
     
    }
           
    for (Loop = 0; Loop < CS_MAX_NUM_MEMORY_TABLE_ENTRIES; Loop++)
    {                
        CS_AppData.DefaultMemoryDefTable[Loop].State              = CS_STATE_EMPTY;
        CS_AppData.DefaultMemoryDefTable[Loop].NumBytesToChecksum = 0;
        CS_AppData.DefaultMemoryDefTable[Loop].StartAddress       = 0;
    }
    
    for (Loop = 0; Loop < CS_MAX_NUM_APP_TABLE_ENTRIES; Loop++)
    {                
        CS_AppData.DefaultAppDefTable[Loop].State              = CS_STATE_EMPTY;
        CS_AppData.DefaultAppDefTable[Loop].Name[0]            = '\0';        
    }


    for (Loop = 0; Loop < CS_MAX_NUM_TABLES_TABLE_ENTRIES; Loop++)
    {                
        CS_AppData.DefaultTablesDefTable[Loop].State              = CS_STATE_EMPTY;
        CS_AppData.DefaultTablesDefTable[Loop].Name[0]            = '\0';
    }
    
    return;
}/* end CS_InitializeDefaultTables */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Set the global variables to the start of the next table      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_GoToNextTable(void)
{    
    if ( CS_AppData.CurrentCSTable < (CS_NUM_TABLES - 1))
    {
        CS_AppData.CurrentCSTable++;
    }
    else
    {
        CS_AppData.CurrentCSTable = 0;
        /* we are back to the beginning of the tables to checksum
         we need to update the pass counter */
        CS_AppData.PassCounter++;
    }
    
    CS_AppData.CurrentEntryInTable = 0;
    return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Get the Results Table Entry info of a table by its name      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean CS_GetTableResTblEntryByName(CS_Res_Tables_Table_Entry_t ** EntryPtr,
                                     char* Name)
{
    CS_Res_Tables_Table_Entry_t           * StartOfResultsTable = NULL;
    CS_Res_Tables_Table_Entry_t           * ResultsEntry        = NULL; 
    int32                                   Loop;
    uint16                                  MaxEntries;
    boolean                                 Status              = FALSE;
    
    
    StartOfResultsTable = CS_AppData.ResTablesTblPtr;
    MaxEntries = CS_MAX_NUM_TABLES_TABLE_ENTRIES;
    
    for (Loop = 0; Loop < MaxEntries; Loop++)
    {
        ResultsEntry = & StartOfResultsTable[Loop];
        
        if (strncmp(ResultsEntry -> Name, Name,CFE_TBL_MAX_FULL_NAME_LEN) == 0  &&
            ResultsEntry -> State != CS_STATE_EMPTY)
        {
            Status      = TRUE;
            *EntryPtr   =  ResultsEntry;
            break;
        }
    }   
    return Status;
}/* end CS_GetTableResTblEntryByName */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Get the Defintion Table Entry info of a table by its name      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean CS_GetTableDefTblEntryByName(CS_Def_Tables_Table_Entry_t ** EntryPtr,
                                     char* Name)
{
    CS_Def_Tables_Table_Entry_t           * StartOfDefinitionTable = NULL;
    CS_Def_Tables_Table_Entry_t           * DefinitionEntry           = NULL; 
    int32                                   Loop;
    uint16                                  MaxEntries;
    boolean                                 Status                 = FALSE;
    
    
    StartOfDefinitionTable = CS_AppData.DefTablesTblPtr;
    MaxEntries = CS_MAX_NUM_TABLES_TABLE_ENTRIES;
    
    for (Loop = 0; Loop < MaxEntries; Loop++)
    {
        DefinitionEntry = & StartOfDefinitionTable[Loop];
        
        if (strncmp(DefinitionEntry -> Name, Name,CFE_TBL_MAX_FULL_NAME_LEN) == 0  &&
            DefinitionEntry -> State != CS_STATE_EMPTY)
        {
            Status      = TRUE;
            *EntryPtr   =  DefinitionEntry;
            break;
        }
    }   
    return Status;
}/* end CS_GetTableDefTblEntryByName */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Get the Results Entry info of an app by its name             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean CS_GetAppResTblEntryByName(CS_Res_App_Table_Entry_t ** EntryPtr,
                                   char                    * Name)
{
    CS_Res_App_Table_Entry_t              * StartOfResultsTable = NULL;
    CS_Res_App_Table_Entry_t              * ResultsEntry        = NULL; 
    int32                                   Loop;
    uint16                                  MaxEntries;
    boolean                                 Status              = FALSE;
    
    StartOfResultsTable = CS_AppData.ResAppTblPtr;
    MaxEntries = CS_MAX_NUM_APP_TABLE_ENTRIES;
    
    for (Loop = 0; Loop < MaxEntries; Loop++)
    {
        ResultsEntry = & StartOfResultsTable[Loop];
        
        if (strncmp(ResultsEntry -> Name, Name, OS_MAX_API_NAME) == 0  &&
            ResultsEntry -> State != CS_STATE_EMPTY)
        {
            Status      = TRUE;
            *EntryPtr   =  ResultsEntry;
            break;
        }
    }   
    return Status;
}/* end CS_GetAppResTblEntryByName */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Get the Definition Entry info of an app by its name          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean CS_GetAppDefTblEntryByName(CS_Def_App_Table_Entry_t ** EntryPtr,
                                   char                    * Name)
{
    CS_Def_App_Table_Entry_t              * StartOfDefinitionTable = NULL;
    CS_Def_App_Table_Entry_t              * DefinitionEntry        = NULL; 
    int32                                   Loop;
    uint16                                  MaxEntries;
    boolean                                 Status                 = FALSE;
    
    StartOfDefinitionTable = CS_AppData.DefAppTblPtr;
    MaxEntries = CS_MAX_NUM_APP_TABLE_ENTRIES;
    
    for (Loop = 0; Loop < MaxEntries; Loop++)
    {
        DefinitionEntry = & StartOfDefinitionTable[Loop];
        
        if (strncmp(DefinitionEntry -> Name, Name, OS_MAX_API_NAME) == 0  &&
            DefinitionEntry -> State != CS_STATE_EMPTY)
        {
            Status      = TRUE;
            *EntryPtr   = DefinitionEntry;
            break;
        }
    }   
    return Status;
}/* end CS_GetAppDefTblEntryByName */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Get the next CS-enabled entry of this table                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean CS_FindEnabledEepromEntry(uint16* EnabledEntry)
{
    CS_Res_EepromMemory_Table_Entry_t       * StartOfResultsTable   = NULL;
    CS_Res_EepromMemory_Table_Entry_t       * ResultsEntry          = NULL;
    boolean                                   EnabledEntries        = TRUE;
    
    
    StartOfResultsTable = CS_AppData.ResEepromTblPtr;    

    ResultsEntry = & StartOfResultsTable[ CS_AppData.CurrentEntryInTable];
    
    while ( ResultsEntry -> State != CS_STATE_ENABLED)
    {
        CS_AppData.CurrentEntryInTable++;
        
        if (CS_AppData.CurrentEntryInTable >= CS_MAX_NUM_EEPROM_TABLE_ENTRIES)
        {
            /* we reached the end no more enabled entries */
            EnabledEntries = FALSE;
            break;
        }
        
        ResultsEntry = & StartOfResultsTable[ CS_AppData.CurrentEntryInTable];
    }/* end while */
    
    *EnabledEntry = CS_AppData.CurrentEntryInTable;
    
    return EnabledEntries;
} /* end CS FindEnabledEepromEntry */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Get the next CS-enabled entry of this table                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean CS_FindEnabledMemoryEntry(uint16* EnabledEntry)
{
    CS_Res_EepromMemory_Table_Entry_t     * StartOfResultsTable     = NULL;
    CS_Res_EepromMemory_Table_Entry_t     * ResultsEntry            = NULL;
    boolean                                 EnabledEntries          = TRUE;
    
    
    StartOfResultsTable = CS_AppData.ResMemoryTblPtr;    
    ResultsEntry        = & StartOfResultsTable[ CS_AppData.CurrentEntryInTable];
    
    while ( ResultsEntry -> State != CS_STATE_ENABLED)
    {
        CS_AppData.CurrentEntryInTable++;
        
        if (CS_AppData.CurrentEntryInTable >= CS_MAX_NUM_MEMORY_TABLE_ENTRIES)
        {
            /* we reached the end no more enabled entries */
            EnabledEntries = FALSE;
            break;
        }
        
        ResultsEntry = & StartOfResultsTable[ CS_AppData.CurrentEntryInTable];
    }/* end while */
    
    *EnabledEntry = CS_AppData.CurrentEntryInTable;
    
    return EnabledEntries;
} /* end CS FindEnabledMemoryEntry */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Get the next CS-enabled entry of this table                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean CS_FindEnabledTablesEntry(uint16* EnabledEntry)
{
    CS_Res_Tables_Table_Entry_t       * StartOfResultsTable     = NULL;
    CS_Res_Tables_Table_Entry_t       * ResultsEntry            = NULL;
    boolean                             EnabledEntries          = TRUE;
    
    StartOfResultsTable = CS_AppData.ResTablesTblPtr;    
    ResultsEntry = & StartOfResultsTable[ CS_AppData.CurrentEntryInTable];

    while ( ResultsEntry -> State != CS_STATE_ENABLED)
    {
        CS_AppData.CurrentEntryInTable++;
        
        if (CS_AppData.CurrentEntryInTable >= CS_MAX_NUM_TABLES_TABLE_ENTRIES)
        {
            /* we reached the end no more enabled entries */
            EnabledEntries = FALSE;

            break;
        }
        
        ResultsEntry = & StartOfResultsTable[ CS_AppData.CurrentEntryInTable];
        
    }/* end while */
    
    *EnabledEntry = CS_AppData.CurrentEntryInTable;
    
    return EnabledEntries;
} /* end CS FindEnabledTablesEntry */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Get the next CS-enabled entry of this table                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean CS_FindEnabledAppEntry(uint16* EnabledEntry)
{
    CS_Res_App_Table_Entry_t          * StartOfResultsTable     = NULL;
    CS_Res_App_Table_Entry_t          * ResultsEntry            = NULL;
    boolean                             EnabledEntries          = TRUE;
    
    StartOfResultsTable = CS_AppData.ResAppTblPtr;    
    ResultsEntry = & StartOfResultsTable[ CS_AppData.CurrentEntryInTable];
    

    while ( ResultsEntry -> State != CS_STATE_ENABLED)
    {
        CS_AppData.CurrentEntryInTable++;
        
        if (CS_AppData.CurrentEntryInTable >= CS_MAX_NUM_APP_TABLE_ENTRIES)
        {
            /* we reached the end no more enabled entries */
            EnabledEntries = FALSE;
            break;
        }
        
        ResultsEntry = & StartOfResultsTable[ CS_AppData.CurrentEntryInTable];
        
    }/* end while */
    
    *EnabledEntry = CS_AppData.CurrentEntryInTable;
    
    
    return EnabledEntries;
} /* end CS FindEnabledAppEntry */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Verify the length of the command                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean CS_VerifyCmdLength(CFE_SB_MsgPtr_t msg, 
                           uint16          ExpectedLength)
{
    CFE_SB_MsgId_t MessageID;
    uint16  CommandCode;
    boolean Result = TRUE;
    uint16  ActualLength = CFE_SB_GetTotalMsgLength(msg);
    
    /* Verify the command packet length */
    if (ExpectedLength != ActualLength)
    {
        CommandCode = CFE_SB_GetCmdCode(msg);
        MessageID= CFE_SB_GetMsgId(msg);
        
        CFE_EVS_SendEvent(CS_LEN_ERR_EID,
                          CFE_EVS_ERROR,
                          "Invalid msg length: ID = 0x%04X, CC = %d, Len = %d, Expected = %d",
                          MessageID,
                          CommandCode,
                          ActualLength,
                          ExpectedLength);
        Result = FALSE;
        CS_AppData.CmdErrCounter++;
    }    
    return(Result);
} /* End of CS_VerifyCmdLength */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Background check cFE core                                       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean CS_BackgroundCfeCore(void)
{
    CS_Res_EepromMemory_Table_Entry_t     * ResultsEntry;
    boolean                                 DoneWithCycle       = FALSE;
    boolean                                 DoneWithEntry       = FALSE;
    uint32                                  ComputedCSValue     = 0;
    int32                                   Status;
    
    if( CS_AppData.CfeCoreCSState == CS_STATE_ENABLED)
    {
        ResultsEntry = & CS_AppData.CfeCoreCodeSeg;
        
        /* Check if the code segment state is enabled (could be disabled for
           a ground-commanded recompute) */
        if (ResultsEntry->State == CS_STATE_ENABLED)
        {
        
            /* If we complete an entry's checksum, this function will update it for us */
            Status = CS_ComputeEepromMemory (ResultsEntry, &ComputedCSValue, &DoneWithEntry);
        
            /* We need to avoid the case of finishing a table, moving on to the next one
             and computing an entry in that table, since it could put us above the 
             maximum bytes per cycle */
            DoneWithCycle = TRUE;
        
            if(Status == CS_ERROR)
            {
                /* we had a miscompare */
            
                CS_AppData.CfeCoreCSErrCounter++;                
            
                CFE_EVS_SendEvent (CS_CFECORE_MISCOMPARE_ERR_EID,
                                   CFE_EVS_ERROR,
                                   "Checksum Failure: cFE Core, Expected: 0x%08X, Calculated: 0x%08X",                                   
                                   (unsigned int)(ResultsEntry -> ComparisonValue),
                                   (unsigned int)ComputedCSValue);
            }
        
            if (DoneWithEntry == TRUE)
            {
                CS_AppData.CurrentEntryInTable ++;
            }
        
            /* only one entry to do */
            if ( CS_AppData.CurrentEntryInTable > 0)
            {
                /* We are done with this table */
                CS_AppData.CfeCoreBaseline = ResultsEntry -> ComparisonValue;
                CS_GoToNextTable();
            }
        }
        else 
        {
            /* Currently disabled, so skip it */
            CS_GoToNextTable();
        }
    }
    else /* Table is disabled, skip it */
    {
        CS_GoToNextTable();
    }
    
    return DoneWithCycle;
}/* end CS_BackgroundCfeCore */    

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Background check OS                                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean CS_BackgroundOS(void)
{
    CS_Res_EepromMemory_Table_Entry_t     * ResultsEntry;
    boolean                                 DoneWithCycle       = FALSE;
    boolean                                 DoneWithEntry       = FALSE;
    uint32                                  ComputedCSValue     = 0;
    int32                                   Status;
    
    if( CS_AppData.OSCSState == CS_STATE_ENABLED)
    {
        ResultsEntry = & CS_AppData.OSCodeSeg;
        
        /* Check if the code segment state is enabled (could be disabled for
         a ground-commanded recompute) */
        if (ResultsEntry->State == CS_STATE_ENABLED)
        {
            /* If we complete an entry's checksum, this function will update it for us */
            Status = CS_ComputeEepromMemory (ResultsEntry, &ComputedCSValue, &DoneWithEntry);
        
            /* We need to avoid the case of finishing a table, moving on to the next one
             and computing an entry in that table, since it could put us above the 
             maximum bytes per cycle */
            DoneWithCycle = TRUE;
        
            if(Status == CS_ERROR)
            {
                /* we had a miscompare */
                CS_AppData.OSCSErrCounter++;                
            
                CFE_EVS_SendEvent (CS_OS_MISCOMPARE_ERR_EID,
                                   CFE_EVS_ERROR,
                                   "Checksum Failure: OS code segment, Expected: 0x%08X, Calculated: 0x%08X",                                   
                                   (unsigned int)(ResultsEntry -> ComparisonValue),
                                   (unsigned int)ComputedCSValue);
            }
        
            if (DoneWithEntry == TRUE)
            {
                CS_AppData.CurrentEntryInTable ++;
            }
        
            /* only one entry to do */
            if ( CS_AppData.CurrentEntryInTable > 0)
            {
                /* We are done with this table */
                CS_AppData.OSBaseline = ResultsEntry -> ComparisonValue;
                CS_GoToNextTable();
            }
        }
        else 
        {
            /* Currently disabled, so skip it */
            CS_GoToNextTable();
        }
    }
    else /* Table is disabled, skip it */
    {
        CS_GoToNextTable();
    }
    return DoneWithCycle;
    
}/* end CS_BackgroundOS */    

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Background check Eeprom                                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean CS_BackgroundEeprom(void)
{
    CS_Res_EepromMemory_Table_Entry_t     * StartOfResultsTable;
    CS_Res_EepromMemory_Table_Entry_t     * ResultsEntry;
    boolean                                 DoneWithCycle           = FALSE;
    boolean                                 DoneWithEntry           = FALSE;
    uint32                                  ComputedCSValue         = 0;
    int32                                   Loop;
    uint32                                  EntireEepromCS;
    uint16                                  CurrEntry;
    int32                                   Status;
    
    if (CS_AppData.EepromCSState == CS_STATE_ENABLED)
    {                   
        if(CS_FindEnabledEepromEntry( &CurrEntry) == TRUE)
        {
            /* we found an enabled entry to checksum */
            StartOfResultsTable = CS_AppData.ResEepromTblPtr;   
            ResultsEntry = & StartOfResultsTable[ CurrEntry];
            
            /* If we complete an entry's checksum, this function will update it for us */
            Status = CS_ComputeEepromMemory (ResultsEntry, &ComputedCSValue, &DoneWithEntry);
            
            /* We need to avoid the case of finishing a table, moving on to the next one
             and computing an entry in that table, since it could put us above the 
             maximum bytes per cycle */
            DoneWithCycle = TRUE;
            
            if(Status == CS_ERROR)
            {
                /* we had a miscompare */
                
                CS_AppData.EepromCSErrCounter++;                
                
                CFE_EVS_SendEvent (CS_EEPROM_MISCOMPARE_ERR_EID,
                                   CFE_EVS_ERROR,
                                   "Checksum Failure: Entry %d in Eeprom Table, Expected: 0x%08X, Calculated: 0x%08X",                                   
                                   CurrEntry,
                                   (unsigned int)(ResultsEntry -> ComparisonValue),
                                   (unsigned int)ComputedCSValue);
            }
            
            if (DoneWithEntry == TRUE)
            {
                CS_AppData.CurrentEntryInTable ++;
            }
                        
            if ( CS_AppData.CurrentEntryInTable >= CS_MAX_NUM_EEPROM_TABLE_ENTRIES)
            {   
                /* Since we are done CS'ing the entire Eeprom table, update the baseline 
                 number for telemetry */
                EntireEepromCS = 0;
                for (Loop = 0; Loop < CS_MAX_NUM_EEPROM_TABLE_ENTRIES; Loop++)
                {
                    EntireEepromCS += CS_AppData.ResEepromTblPtr[Loop].ComparisonValue;
                }
                
                CS_AppData.EepromBaseline = EntireEepromCS;
                
                /* We are done with this table */
                CS_GoToNextTable();
            }
        }
        else
        {
            /* If we don't have a full table, the above set of code won't get
             executed, so we do it if there aren't any more full entries left */
            if ( CS_AppData.CurrentEntryInTable >= CS_MAX_NUM_EEPROM_TABLE_ENTRIES)
            {   
                /* Since we are done CS'ing the entire Eeprom table, update the baseline 
                 number for telemetry */
                EntireEepromCS = 0;
                for (Loop = 0; Loop < CS_MAX_NUM_EEPROM_TABLE_ENTRIES; Loop++)
                {
                    EntireEepromCS += CS_AppData.ResEepromTblPtr[Loop].ComparisonValue;
                }                
                CS_AppData.EepromBaseline = EntireEepromCS;
            }
            
            /* There are no enabled entries in this table */
            CS_GoToNextTable();
        }
    }
    else /* Table is disabled, skip it */
    {
        CS_GoToNextTable();
    }
    
    return DoneWithCycle;
    
}/* end CS_BackgroundEeprom */  

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Background check Memory                                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean CS_BackgroundMemory(void)
{
    CS_Res_EepromMemory_Table_Entry_t     * StartOfResultsTable;
    CS_Res_EepromMemory_Table_Entry_t     * ResultsEntry;
    boolean                                 DoneWithCycle           = FALSE;
    boolean                                 DoneWithEntry           = FALSE;
    uint32                                  ComputedCSValue         = 0;
    uint16                                  CurrEntry;
    int32                                   Status;
    
    if (CS_AppData.MemoryCSState == CS_STATE_ENABLED)
    {                   
        /* If we complete an entry's checksum, this function will update it for us */
        
        if (CS_FindEnabledMemoryEntry( &CurrEntry) == TRUE)
        {
            /* we found an enabled entry to checksum */
            StartOfResultsTable = CS_AppData.ResMemoryTblPtr;   
            ResultsEntry = & StartOfResultsTable[ CurrEntry];
            
            /* If we complete an entry's checksum, this function will update it for us */
            Status = CS_ComputeEepromMemory (ResultsEntry, &ComputedCSValue, &DoneWithEntry);
            
            /* We need to avoid the case of finishing a table, moving on to the next one
             and computing an entry in that table, since it could put us above the 
             maximum bytes per cycle */
            DoneWithCycle = TRUE;
            
            if(Status == CS_ERROR)
            {
                /* we had a miscompare */
                
                CS_AppData.MemoryCSErrCounter++;                
                
                CFE_EVS_SendEvent (CS_MEMORY_MISCOMPARE_ERR_EID,
                                   CFE_EVS_ERROR,
                                   "Checksum Failure: Entry %d in Memory Table, Expected: 0x%08X, Calculated: 0x%08X",                                   
                                   CurrEntry,
                                   (unsigned int)(ResultsEntry -> ComparisonValue),
                                   (unsigned int)ComputedCSValue);
                 
                CS_AppData.CurrentEntryInTable ++;
            }
            
            if (DoneWithEntry == TRUE)
            {
                CS_AppData.CurrentEntryInTable ++;
            }
            
            if ( CS_AppData.CurrentEntryInTable >= CS_MAX_NUM_MEMORY_TABLE_ENTRIES)
            {
                /* We are done with this table */
                CS_GoToNextTable();
            }
        }
        else /* since there aren't any enabled entries in this table, go to the next one */
        {
            CS_GoToNextTable();
        }
    }
    else /* Table is disabled, skip it */
    {
        CS_GoToNextTable();
    }
        
    return DoneWithCycle;
    
}/* end CS_BackgroundMemory */  

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Background check Tables                                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean CS_BackgroundTables(void)
{
    CS_Res_Tables_Table_Entry_t           * StartOfTablesResultsTable;
    CS_Res_Tables_Table_Entry_t           * TablesResultsEntry;
    boolean                                 DoneWithCycle           = FALSE;
    boolean                                 DoneWithEntry           = FALSE;
    uint32                                  ComputedCSValue         = 0;
    uint16                                  CurrEntry;
    int32                                   Status;
    
    if (CS_AppData.TablesCSState == CS_STATE_ENABLED)
    {                                        
        /* If we complete an entry's checksum, this function will update it for us */
        
        if (CS_FindEnabledTablesEntry( &CurrEntry) == TRUE)
        {
            /* we found an enabled entry to checksum */
            
            StartOfTablesResultsTable = CS_AppData.ResTablesTblPtr;   
            TablesResultsEntry = & StartOfTablesResultsTable[ CurrEntry];
            
            Status = CS_ComputeTables (TablesResultsEntry, &ComputedCSValue, &DoneWithEntry);
            
            /* We need to avoid the case of finishing a table, moving on to the next one
             and computing an entry in that table, since  it could put us above the 
             maximum bytes per cycle */
            DoneWithCycle = TRUE;
            
            if(Status == CS_ERROR)
            {
                /* we had a miscompare */
                CS_AppData.TablesCSErrCounter++;                
                
                CFE_EVS_SendEvent (CS_TABLES_MISCOMPARE_ERR_EID,
                                   CFE_EVS_ERROR,
                                   "Checksum Failure: Table %s, Expected: 0x%08X, Calculated: 0x%08X",                                   
                                   TablesResultsEntry -> Name,
                                   (unsigned int)(TablesResultsEntry -> ComparisonValue),
                                   (unsigned int)ComputedCSValue);
            }
            
            if(Status == CS_ERR_NOT_FOUND)
            {
                CFE_EVS_SendEvent (CS_COMPUTE_TABLES_NOT_FOUND_ERR_EID,
                                   CFE_EVS_ERROR,
                                   "Tables table computing: Table %s could not be found, skipping", 
                                   TablesResultsEntry -> Name);
                
                CS_AppData.CurrentEntryInTable ++;
            }
            
            if (DoneWithEntry == TRUE)
            {
                CS_AppData.CurrentEntryInTable ++;
            }
            
            if ( CS_AppData.CurrentEntryInTable >= CS_MAX_NUM_TABLES_TABLE_ENTRIES)
            {
                /* We are done with this table */
                CS_GoToNextTable();
            }
        }
        else /* since there aren't any enabled entries in this table, go to the next one */
        {
            CS_GoToNextTable();
        }
    }
    else /* Table is disabled, skip it */
    {
        CS_GoToNextTable();
    }
    
    return DoneWithCycle;
    
}/* end CS_BackgroundTables */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Background check App                                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean CS_BackgroundApp(void)
{
    CS_Res_App_Table_Entry_t              * StartOfAppResultsTable;
    CS_Res_App_Table_Entry_t              * AppResultsEntry;   
    boolean                                 DoneWithCycle           = FALSE;
    boolean                                 DoneWithEntry           = FALSE;
    uint32                                  ComputedCSValue         = 0;
    uint16                                  CurrEntry;
    int32                                   Status;

    if (CS_AppData.AppCSState == CS_STATE_ENABLED)
    {                    
        if (CS_FindEnabledAppEntry( &CurrEntry) == TRUE)
        {
            /* we found an enabled entry to checksum */
            
            StartOfAppResultsTable = CS_AppData.ResAppTblPtr;   
            AppResultsEntry = & StartOfAppResultsTable[  CurrEntry];
            
            /* If we complete an entry's checksum, this function will update it for us */
            Status = CS_ComputeApp (AppResultsEntry, &ComputedCSValue, &DoneWithEntry);
            
            /* We need to avoid the case of finishing a table, moving on to the next one
             and computing an entry in that table, since it could put us above the 
             maximum bytes per cycle */
            DoneWithCycle = TRUE;
            
            if(Status == CS_ERROR)
            {
                /* we had a miscompare */
                CS_AppData.AppCSErrCounter++;                
                
                CFE_EVS_SendEvent (CS_APP_MISCOMPARE_ERR_EID, CFE_EVS_ERROR,
                                   "Checksum Failure: Application %s, Expected: 0x%08X, Calculated: 0x%08X",                                   
                                   AppResultsEntry -> Name,
                                   (unsigned int)(AppResultsEntry -> ComparisonValue),
                                   (unsigned int)ComputedCSValue);
            }
            
            if(Status == CS_ERR_NOT_FOUND)
            {
                CFE_EVS_SendEvent (CS_COMPUTE_APP_NOT_FOUND_ERR_EID,
                                   CFE_EVS_ERROR,
                                   "App table computing: App %s could not be found, skipping", 
                                   AppResultsEntry -> Name);
                
                CS_AppData.CurrentEntryInTable ++;

            }
            
            if (DoneWithEntry == TRUE)
            {
                CS_AppData.CurrentEntryInTable ++;
            }
            
            if ( CS_AppData.CurrentEntryInTable >= CS_MAX_NUM_APP_TABLE_ENTRIES)
            {
                /* We are done with this table */
                CS_GoToNextTable();
            }
        }
        else /* since there aren't any enabled entries in this table, go to the next one */
        {
            CS_GoToNextTable();
        }
    }
    else /* Table is disabled, skip it */
    {
        CS_GoToNextTable();
    }    
    return DoneWithCycle;
    
}/* end CS_BackgroundApp */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset Tables result table entry after a CS table gets modified  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ResetTablesTblResultEntry(CS_Res_Tables_Table_Entry_t *TablesTblResultEntry)
{
    /* Ptr will be NULL if this CS table is not listed in the CS Tables table */
    if (TablesTblResultEntry != (CS_Res_Tables_Table_Entry_t *) NULL)
    {
        TablesTblResultEntry -> ByteOffset = 0;
        TablesTblResultEntry -> TempChecksumValue = 0;
        TablesTblResultEntry -> ComputedYet = FALSE;
    }

    return;

}/* end CS_ResetTablesTblResultEntry */

/************************/
/*  End of File Comment */
/************************/
