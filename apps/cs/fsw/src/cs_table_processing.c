/************************************************************************
 ** File:
 **   $Id: cs_table_processing.c 1.6 2017/02/16 15:33:18EST mdeschu Exp  $
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
 **   The CFS Checksum (CS) Application's table updating functions 
 **
 *************************************************************************/

/*************************************************************************
 **
 ** Include section
 **
 **************************************************************************/
#include "cfe.h"
#include "cs_app.h"
#include "cs_events.h"
#include "cs_tbldefs.h"
#include "cs_utils.h"
#include <string.h>

/*************************************************************************
 **
 ** Local function prototypes
 **
 **************************************************************************/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Validation Callback function for Eeprom Table                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CS_ValidateEepromChecksumDefinitionTable (void * TblPtr)
{
    int32                                       Result       = CFE_SUCCESS;
    int32                                       Status       = OS_ERROR;
    CS_Def_EepromMemory_Table_Entry_t         * StartOfTable = NULL;
    CS_Def_EepromMemory_Table_Entry_t         * OuterEntry   = NULL;
    int32                                       OuterLoop    = 0;
    uint32                                      StateField   = 0;
    uint32                                      Address      = 0;
    uint32                                      Size         = 0;
    int32                                       GoodCount    = 0;
    int32                                       BadCount     = 0;
    int32                                       EmptyCount   = 0;
    
    StartOfTable = (CS_Def_EepromMemory_Table_Entry_t *) TblPtr;
    
    for (OuterLoop = 0; OuterLoop < CS_MAX_NUM_EEPROM_TABLE_ENTRIES; OuterLoop++)
    {
        OuterEntry = & StartOfTable [OuterLoop];
        
        StateField = OuterEntry->State;
        Address    = OuterEntry->StartAddress;
        Size       = OuterEntry->NumBytesToChecksum;
        
        if ((StateField == CS_STATE_EMPTY) ||
            (StateField == CS_STATE_ENABLED) ||
            (StateField ==  CS_STATE_DISABLED))
        {
            /* If the StateField is within this range, it's check if it's not empty. */
            if (StateField == CS_STATE_DISABLED || StateField == CS_STATE_ENABLED)
            {
                Status = CFE_PSP_MemValidateRange(Address,Size,CFE_PSP_MEM_EEPROM);
                if (Status != OS_SUCCESS)
                {
                    BadCount++;
                    if (Result != CS_TABLE_ERROR)
                    {
                        CFE_EVS_SendEvent (CS_VAL_EEPROM_RANGE_ERR_EID,
                                           CFE_EVS_ERROR,
                                           "Eeprom Table Validate: Illegal checksum range found in Entry ID %d, CFE_PSP_MemValidateRange returned: 0x%08X",
                                           (int)OuterLoop,
                                           (unsigned int)Status);
                        Result = CS_TABLE_ERROR;
                    }
                }
                else 
                {
                    /* Valid range for non-empty entry */
                    GoodCount++;
                }
            }
            else
            {
                /* Entry is marked as empty */
                EmptyCount++;    
            }
        }
        else
        {
            /* Invalid state definition */
            BadCount++;
            if (Result != CS_TABLE_ERROR)
            {
                CFE_EVS_SendEvent (CS_VAL_EEPROM_STATE_ERR_EID,
                                   CFE_EVS_ERROR,
                                   "Eeprom Table Validate: Illegal State Field (0x%04X) found in Entry ID %d",
                                   (unsigned short)StateField,
                                   (int)OuterLoop);
                Result = CS_TABLE_ERROR;            
            }
        }
    }   /* for (OuterLoop = 0; OuterLoop < CS_MAX_NUM_EEPROM_TABLE_ENTRIES; OuterLoop++) */
    
    CFE_EVS_SendEvent(CS_VAL_EEPROM_INF_EID,
                      CFE_EVS_INFORMATION,
                      "CS Eeprom Table verification results: good = %d, bad = %d, unused = %d",
                      (int)GoodCount,
                      (int)BadCount,
                      (int)EmptyCount);

    return (Result);
}   /* CS_ValidateEEPROMCheckSumDefinitionTable */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Validation Callback function for Memory Table                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CS_ValidateMemoryChecksumDefinitionTable (void * TblPtr)
{
    int32                                       Result       = CFE_SUCCESS;
    int32                                       Status       = OS_ERROR;
    CS_Def_EepromMemory_Table_Entry_t         * StartOfTable = NULL;
    CS_Def_EepromMemory_Table_Entry_t         * OuterEntry   = NULL;
    int32                                       OuterLoop    = 0;
    uint32                                      StateField   = 0;
    uint32                                      Address      = 0;
    uint32                                      Size         = 0;
    int32                                       GoodCount    = 0;
    int32                                       BadCount     = 0;
    int32                                       EmptyCount   = 0;
    
    StartOfTable = (CS_Def_EepromMemory_Table_Entry_t *) TblPtr;
    
    Result = CFE_SUCCESS;
    
    for (OuterLoop = 0; OuterLoop < CS_MAX_NUM_MEMORY_TABLE_ENTRIES; OuterLoop++)
    {
        OuterEntry = & StartOfTable [OuterLoop];
        
        StateField = OuterEntry -> State;
        Address    = OuterEntry -> StartAddress;
        Size       = OuterEntry -> NumBytesToChecksum;
        
        if ((StateField == CS_STATE_EMPTY)  ||
            (StateField == CS_STATE_ENABLED) ||
            (StateField ==  CS_STATE_DISABLED))
        {
            /* If the StateField is within this range, check if it's not empty. */
            if (StateField == CS_STATE_DISABLED || StateField == CS_STATE_ENABLED)
            {
                Status = CFE_PSP_MemValidateRange(Address,Size,CFE_PSP_MEM_ANY);
                if (Status != OS_SUCCESS)
                {
                    BadCount++;
                    if (Result != CS_TABLE_ERROR)
                    {
                        CFE_EVS_SendEvent (CS_VAL_MEMORY_RANGE_ERR_EID,
                                           CFE_EVS_ERROR,
                                           "Memory Table Validate: Illegal checksum range found in Entry ID %d, CFE_PSP_MemValidateRange returned: 0x%08X",
                                           (int)OuterLoop,
                                           (unsigned int)Status);
                        Result = CS_TABLE_ERROR;
                    }
                }
                else 
                {
                    /* Valid range for non-empty entry */
                    GoodCount++;
                }
            }
            else
            {
                /* Entry is marked as empty */
                EmptyCount++;    
            }
        }
        else
        {
            /* Invalid state definition */
            BadCount++;
            if (Result != CS_TABLE_ERROR)
            {
                CFE_EVS_SendEvent (CS_VAL_MEMORY_STATE_ERR_EID, CFE_EVS_ERROR,
                                   "Memory Table Validate: Illegal State Field (0x%04X) found in Entry ID %d",
                                   (unsigned short)StateField, (int)OuterLoop);
            
                Result = CS_TABLE_ERROR;
            }
        }
                
    }   /* for (OuterLoop = 0; OuterLoop < CS_MAX_NUM_MEMORY_TABLE_ENTRIES; OuterLoop++) */
    
    CFE_EVS_SendEvent(CS_VAL_MEMORY_INF_EID,
                      CFE_EVS_INFORMATION,
                      "CS Memory Table verification results: good = %d, bad = %d, unused = %d",
                      (int)GoodCount,
                      (int)BadCount,
                      (int)EmptyCount);
        
    return (Result);
}   /* CS_ValidateMemoryCheckSumDefinitionTable */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Validation Callback function for Tables Table                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CS_ValidateTablesChecksumDefinitionTable (void * TblPtr)
{
    int32                                       Result         = CFE_SUCCESS;
    CS_Def_Tables_Table_Entry_t               * StartOfTable   = NULL;
    CS_Def_Tables_Table_Entry_t               * OuterEntry     = NULL;
    int32                                       OuterLoop      = 0;
    int32                                       InnerLoop      = 0;
    uint32                                      StateField     = 0;
    int32                                       GoodCount      = 0;
    int32                                       BadCount       = 0;
    int32                                       EmptyCount     = 0;
    boolean                                     DuplicateFound = FALSE;
    
    StartOfTable = (CS_Def_Tables_Table_Entry_t *) TblPtr;
    
    for (OuterLoop = 0; OuterLoop < CS_MAX_NUM_TABLES_TABLE_ENTRIES; OuterLoop++)
    {
        OuterEntry = & StartOfTable [OuterLoop];
        
        StateField = OuterEntry -> State;
        
        /* Check for non-zero length for table name */
        if (strlen(OuterEntry->Name) != 0)
        {
            /* Verify valid state definition */
            if (((StateField == CS_STATE_EMPTY) || 
                (StateField == CS_STATE_ENABLED) ||
                (StateField ==  CS_STATE_DISABLED) ))
            {
                DuplicateFound = FALSE;
                
                /* Verify the name field is not duplicated */
                for (InnerLoop = OuterLoop+1; InnerLoop < CS_MAX_NUM_TABLES_TABLE_ENTRIES; InnerLoop++)
                {
                    if (strncmp(OuterEntry->Name, (&StartOfTable[InnerLoop])->Name, CFE_TBL_MAX_FULL_NAME_LEN) == 0)
                    {
                        if (DuplicateFound != TRUE)
                        {
                            DuplicateFound = TRUE;
                            BadCount++;
                        }
                        
                        if (Result != CS_TABLE_ERROR)
                        {
                            /* Duplicate name entry found */
                            CFE_EVS_SendEvent (CS_VAL_TABLES_DEF_TBL_DUPL_ERR_EID,
                                               CFE_EVS_ERROR,
                                               "CS Tables Table Validate: Duplicate Name (%s) found at entries %d and %d",
                                               OuterEntry -> Name,
                                               (int)InnerLoop,
                                               (int)OuterLoop);
                        
                            Result = CS_TABLE_ERROR;
                        }
                    }
                }
                
                /* Increment success/empty counter if name wasn't duplicated */
                if (DuplicateFound != TRUE)
                {
                    if (StateField != CS_STATE_EMPTY)
                    {
                        GoodCount++;
                    }
                    else
                    {
                        EmptyCount++;
                    }
                }
            }
            else
            {
                if (Result != CS_TABLE_ERROR)
                {
                    CFE_EVS_SendEvent (CS_VAL_TABLES_STATE_ERR_EID,
                                       CFE_EVS_ERROR,
                                       "CS Tables Table Validate: Illegal State Field (0x%04X) found with name %s",
                                       (unsigned short)StateField, 
                                       OuterEntry -> Name);
            
                    Result = CS_TABLE_ERROR;
                    BadCount++;
                }
            }
        }
        else 
        {
            /* Only entries marked as Empty can have zero-length names */
            if (StateField != CS_STATE_EMPTY)
            {
                /* Bad state for empty name */
                if (Result != CS_TABLE_ERROR)
                {
                    CFE_EVS_SendEvent (CS_VAL_TABLES_DEF_TBL_ZERO_NAME_ERR_EID,
                                       CFE_EVS_ERROR,
                                       "CS Tables Table Validate: Illegal State (0x%04X) with empty name at entry %d",
                                       (unsigned short)StateField,
                                       (int)OuterLoop);
                    
                    Result = CS_TABLE_ERROR;
                    BadCount++;
                }
            }
            else
            {
                EmptyCount++;
            }
        }

    }   /* for (OuterLoop = 0; OuterLoop < CS_MAX_NUM_TABLES_TABLE_ENTRIES; OuterLoop++) */
    
    CFE_EVS_SendEvent(CS_VAL_TABLES_INF_EID,
                      CFE_EVS_INFORMATION,
                      "CS Tables Table verification results: good = %d, bad = %d, unused = %d",
                      (int)GoodCount,
                      (int)BadCount,
                      (int)EmptyCount);
        
    return (Result);
}   /* CS_ValidateTablesCheckSumDefinitionTable */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Validation Callback function for App Table                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CS_ValidateAppChecksumDefinitionTable (void * TblPtr)
{
    int32                                       Result         = CFE_SUCCESS;
    CS_Def_App_Table_Entry_t                  * StartOfTable   = NULL;
    CS_Def_App_Table_Entry_t                  * OuterEntry     = NULL;
    int32                                       OuterLoop      = 0;
    int32                                       InnerLoop      = 0;
    uint32                                      StateField     = 0;
    int32                                       GoodCount      = 0;
    int32                                       BadCount       = 0;
    int32                                       EmptyCount     = 0;
    boolean                                     DuplicateFound = FALSE;
    
    StartOfTable = (CS_Def_App_Table_Entry_t *) TblPtr;
    
    for (OuterLoop = 0; OuterLoop < CS_MAX_NUM_APP_TABLE_ENTRIES; OuterLoop++)
    {
        OuterEntry = & StartOfTable [OuterLoop];
        
        StateField = OuterEntry -> State;

        /* Check for non-zero length for table name */
        if (strlen(OuterEntry->Name) != 0)
        {
            /* Verify valid state definition */
            if (((StateField == CS_STATE_EMPTY) || 
                 (StateField == CS_STATE_ENABLED) ||
                 (StateField ==  CS_STATE_DISABLED) ))
            {
                DuplicateFound = FALSE;
                
                /* Verify the name field is not duplicated */
                for (InnerLoop = OuterLoop+1; InnerLoop < CS_MAX_NUM_APP_TABLE_ENTRIES; InnerLoop++)
                {
                    if (strncmp(OuterEntry->Name, (&StartOfTable[InnerLoop])->Name, CFE_TBL_MAX_FULL_NAME_LEN) == 0)
                    {
                        if (DuplicateFound != TRUE)
                        {
                            DuplicateFound = TRUE;
                            BadCount++;
                        }
                        
                        if (Result != CS_TABLE_ERROR)
                        {
                            /* Duplicate name entry found */
                            CFE_EVS_SendEvent (CS_VAL_APP_DEF_TBL_DUPL_ERR_EID,
                                               CFE_EVS_ERROR,
                                               "CS Apps Table Validate: Duplicate Name (%s) found at entries %d and %d",
                                               OuterEntry -> Name,
                                               (int)InnerLoop,
                                               (int)OuterLoop);
                            
                            Result = CS_TABLE_ERROR;
                        }
                    }
                }
                
                /* Increment success/empty counter if name wasn't duplicated */
                if (DuplicateFound != TRUE)
                {
                    if (StateField != CS_STATE_EMPTY)
                    {
                        GoodCount++;
                    }
                    else
                    {
                        EmptyCount++;
                    }
                }
            }
            else
            {
                if (Result != CS_TABLE_ERROR)
                {
                    CFE_EVS_SendEvent (CS_VAL_APP_STATE_ERR_EID,
                                       CFE_EVS_ERROR,
                                       "CS Apps Table Validate: Illegal State Field (0x%04X) found with name %s",
                                       (unsigned short)StateField, 
                                       OuterEntry -> Name);
                    
                    Result = CS_TABLE_ERROR;
                    BadCount++;
                }
            }
        }
        else 
        {
            /* Only entries marked as Empty can have zero-length names */
            if (StateField != CS_STATE_EMPTY)
            {
                /* Bad state for empty name */
                if (Result != CS_TABLE_ERROR)
                {
                    CFE_EVS_SendEvent (CS_VAL_APP_DEF_TBL_ZERO_NAME_ERR_EID,
                                       CFE_EVS_ERROR,
                                       "CS Apps Table Validate: Illegal State (0x%04X) with empty name at entry %d",
                                       (unsigned short)StateField,
                                       (int)OuterLoop);
                    
                    Result = CS_TABLE_ERROR;
                    BadCount++;
                }
            }
            else
            {
                EmptyCount++;
            }
        }
        
    }   /* for (OuterLoop = 0; OuterLoop < CS_MAX_NUM_APPS_TABLE_ENTRIES; OuterLoop++) */
    
    CFE_EVS_SendEvent(CS_VAL_APP_INF_EID,
                      CFE_EVS_INFORMATION,
                      "CS Apps Table verification results: good = %d, bad = %d, unused = %d",
                      (int)GoodCount,
                      (int)BadCount,
                      (int)EmptyCount);
    
    return (Result);
}   /* CS_ValidateAppCheckSumDefinitionTable */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS  processing new definition tables for Eeprom or Memory       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ProcessNewEepromMemoryDefinitionTable (CS_Def_EepromMemory_Table_Entry_t   * DefinitionTblPtr, 
                                               CS_Res_EepromMemory_Table_Entry_t      * ResultsTblPtr,
                                                uint16 NumEntries, 
                                                uint16 Table)
{
    CS_Def_EepromMemory_Table_Entry_t         * StartOfDefTable     = NULL;
    CS_Def_EepromMemory_Table_Entry_t         * DefEntry            = NULL;
    CS_Res_EepromMemory_Table_Entry_t         * StartOfResultsTable = NULL;
    CS_Res_EepromMemory_Table_Entry_t         * ResultsEntry        = NULL;
    uint16                                      Loop                = 0;
    uint16                                      NumRegionsInTable   = 0;
    uint16                                      PreviousState       = CS_STATE_EMPTY;
    char                                        TableType[CS_TABLETYPE_NAME_SIZE];
    
    CFE_PSP_MemCpy(&StartOfResultsTable, ResultsTblPtr,    sizeof(StartOfResultsTable));
    CFE_PSP_MemCpy(&StartOfDefTable,     DefinitionTblPtr, sizeof(StartOfDefTable));   

    strncpy(&TableType[0], "Undef Tbl", CS_TABLETYPE_NAME_SIZE);  /* Init the table type string */
        
    /* We don't want to be doing chekcksums while changing the table out */
    if( Table == CS_EEPROM_TABLE)
    {
        PreviousState = CS_AppData.EepromCSState;
        CS_AppData.EepromCSState = CS_STATE_DISABLED;
    }
    if( Table == CS_MEMORY_TABLE)
    {
        PreviousState = CS_AppData.MemoryCSState;
        CS_AppData.MemoryCSState = CS_STATE_DISABLED;
    }
    
    for (Loop = 0; Loop < NumEntries; Loop++)
    {
        DefEntry = & (StartOfDefTable [Loop]);
        ResultsEntry =  &(StartOfResultsTable [Loop]);
        
        if (DefEntry -> State != CS_STATE_EMPTY )
        {
            /* This is just a real simple test, because all of the exception handling
             has already been done by the Validation routine above */
            NumRegionsInTable++;
                        
            ResultsEntry -> State              = DefEntry -> State;
            ResultsEntry -> ComputedYet        = FALSE;
            ResultsEntry -> NumBytesToChecksum = DefEntry -> NumBytesToChecksum;
            ResultsEntry -> ComparisonValue    = 0;
            ResultsEntry -> ByteOffset         = 0;
            ResultsEntry -> TempChecksumValue  = 0;
            ResultsEntry -> StartAddress       = DefEntry -> StartAddress;
        }
        else
        {
            ResultsEntry -> State              = CS_STATE_EMPTY;
            ResultsEntry -> ComputedYet        = FALSE;
            ResultsEntry -> NumBytesToChecksum = 0;
            ResultsEntry -> ComparisonValue    = 0;
            ResultsEntry -> ByteOffset         = 0;
            ResultsEntry -> TempChecksumValue  = 0;
            ResultsEntry -> StartAddress       = 0; 
        }
    }
    
    /* Reset the table back to the original checksumming state */
    if( Table == CS_EEPROM_TABLE)
    {
        CS_AppData.EepromCSState = PreviousState;
        CS_ResetTablesTblResultEntry(CS_AppData.EepResTablesTblPtr);
    }

    if( Table == CS_MEMORY_TABLE)
    {
        CS_AppData.MemoryCSState = PreviousState;
        CS_ResetTablesTblResultEntry(CS_AppData.MemResTablesTblPtr);
    }
    
    if (NumRegionsInTable == 0)
    {
        if( Table == CS_EEPROM_TABLE)
        {
            strncpy(&TableType[0], "Eeprom", CS_TABLETYPE_NAME_SIZE);
        }
        if( Table == CS_MEMORY_TABLE)
        {
            strncpy(&TableType[0], "Memory", CS_TABLETYPE_NAME_SIZE);
        }
            
        CFE_EVS_SendEvent (CS_PROCESS_EEPROM_MEMORY_NO_ENTRIES_INF_EID,
                           CFE_EVS_INFORMATION,
                           "CS %s Table: No valid entries in the table",
                           TableType);
    }
    return;
}   /* end of CS_ProcessNewEepromMemoryDefinitionTable () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS processing new definition tables for Tables                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ProcessNewTablesDefinitionTable    (CS_Def_Tables_Table_Entry_t      * DefinitionTblPtr, 
                                            CS_Res_Tables_Table_Entry_t      * ResultsTblPtr)
{
    CS_Def_Tables_Table_Entry_t               * StartOfDefTable     = NULL;
    CS_Def_Tables_Table_Entry_t               * DefEntry            = NULL;
    CS_Res_Tables_Table_Entry_t               * StartOfResultsTable = NULL;
    CS_Res_Tables_Table_Entry_t               * ResultsEntry        = NULL;
    uint16                                      Loop                = 0;
    uint16                                      NumRegionsInTable   = 0;
    uint16                                      PreviousState       = CS_STATE_EMPTY;
    uint32                                      AppID               = 0;
    CFE_TBL_Handle_t                            TableHandle         = CFE_TBL_BAD_TABLE_HANDLE;
    boolean                                     Owned               = FALSE;
    uint16                                      DefNameIndex        = 0;
    uint16                                      AppNameIndex        = 0;
    uint16                                      TableNameIndex      = 0;
    char                                        AppName [OS_MAX_API_NAME];
    char                                        TableAppName [OS_MAX_API_NAME];
    char                                        TableTableName [CFE_TBL_MAX_NAME_LENGTH];
    
    CFE_PSP_MemCpy(&StartOfResultsTable, ResultsTblPtr,    sizeof(StartOfResultsTable));
    CFE_PSP_MemCpy(&StartOfDefTable,     DefinitionTblPtr, sizeof(StartOfDefTable));  
    
    CFE_ES_GetAppID(&AppID);
    CFE_ES_GetAppName( AppName, AppID, OS_MAX_API_NAME);
    
    /* We don't want to be doing chekcksums while changing the table out */
    PreviousState = CS_AppData.TablesCSState;
    CS_AppData.TablesCSState = CS_STATE_DISABLED;

    /* Assume none of the CS tables are listed in the new Tables table */
    CS_AppData.EepResTablesTblPtr = NULL;
    CS_AppData.MemResTablesTblPtr = NULL;
    CS_AppData.AppResTablesTblPtr = NULL;
    CS_AppData.TblResTablesTblPtr = NULL;

    for (Loop = 0; Loop < CS_MAX_NUM_TABLES_TABLE_ENTRIES; Loop++)
    {
        DefEntry = & (StartOfDefTable [Loop]);
        
        ResultsEntry = & (StartOfResultsTable [Loop]);
        
        if (DefEntry -> State != CS_STATE_EMPTY)
        {
            /* initialize buffer index values */
            DefNameIndex = 0;
            AppNameIndex = 0;
            TableNameIndex = 0;

            /* extract application name from CS definition table entry */
            while ((AppNameIndex < OS_MAX_API_NAME) &&
                   (DefEntry->Name[DefNameIndex] != '\0') && (DefEntry->Name[DefNameIndex] != '.'))
            {
                TableAppName[AppNameIndex++] = DefEntry->Name[DefNameIndex++];
            }

            /* limit application name length to add string terminator */
            if (AppNameIndex == OS_MAX_API_NAME)
            {
                AppNameIndex = OS_MAX_API_NAME - 1;
            }

            /* add string terminator to application name */
            TableAppName[AppNameIndex] = '\0';

            /* move entry index to the dot, string terminator, or end of table entry */
            while ((DefNameIndex < CFE_TBL_MAX_FULL_NAME_LEN) &&
                   (DefEntry->Name[DefNameIndex] != '\0') &&
                   (DefEntry->Name[DefNameIndex] != '.'))
            {
                DefNameIndex++;
            }

            /* move entry index forward past the dot */
            if ((DefNameIndex < CFE_TBL_MAX_FULL_NAME_LEN) &&
                (DefEntry->Name[DefNameIndex] == '.'))
            {
                DefNameIndex++;
            }

            /* extract table name from CS definition table entry */
            while ((DefNameIndex < CFE_TBL_MAX_FULL_NAME_LEN) &&
                   (TableNameIndex < CFE_TBL_MAX_NAME_LENGTH) &&
                   (DefEntry->Name[DefNameIndex] != '\0'))
            {
                TableTableName[TableNameIndex++] = DefEntry->Name[DefNameIndex++];
            }

            /* limit table name length to add string terminator */
            if (TableNameIndex == CFE_TBL_MAX_NAME_LENGTH)
            {
                TableNameIndex = CFE_TBL_MAX_NAME_LENGTH - 1;
            }

            /* add string terminator to table name */
            TableTableName[TableNameIndex] = '\0';
          
            
            
            
            
            

            TableHandle = CFE_TBL_BAD_TABLE_HANDLE;
            Owned = FALSE;
            
            /* if the table's owner's name is CS */
            if( strncmp (TableAppName, AppName, OS_MAX_API_NAME) == 0)
            {
                if (strncmp (TableTableName, CS_DEF_EEPROM_TABLE_NAME, CFE_TBL_MAX_NAME_LENGTH) == 0)
                {
                    TableHandle = CS_AppData.DefEepromTableHandle;
                    CS_AppData.EepResTablesTblPtr = ResultsEntry;
                    Owned = TRUE;
                }
                if (strncmp (TableTableName, CS_DEF_MEMORY_TABLE_NAME, CFE_TBL_MAX_NAME_LENGTH) == 0)
                {
                    TableHandle = CS_AppData.DefMemoryTableHandle;
                    CS_AppData.MemResTablesTblPtr = ResultsEntry;
                    Owned = TRUE;
                }
                if (strncmp (TableTableName, CS_DEF_TABLES_TABLE_NAME ,CFE_TBL_MAX_NAME_LENGTH) == 0)
                {
                    TableHandle = CS_AppData.DefTablesTableHandle;
                    CS_AppData.TblResTablesTblPtr = ResultsEntry;
                    Owned = TRUE;
                }
                if (strncmp (TableTableName,CS_DEF_APP_TABLE_NAME ,CFE_TBL_MAX_NAME_LENGTH) == 0)
                {
                    TableHandle = CS_AppData.DefAppTableHandle;
                    CS_AppData.AppResTablesTblPtr = ResultsEntry;
                    Owned = TRUE;
                }
                if (strncmp (TableTableName,CS_RESULTS_EEPROM_TABLE_NAME ,CFE_TBL_MAX_NAME_LENGTH) == 0)
                {
                    TableHandle = CS_AppData.ResEepromTableHandle;
                    Owned = TRUE;
                }
                if (strncmp (TableTableName,CS_RESULTS_MEMORY_TABLE_NAME ,CFE_TBL_MAX_NAME_LENGTH) == 0)
                {
                    TableHandle = CS_AppData.ResMemoryTableHandle;
                    Owned = TRUE;
                }
                if (strncmp (TableTableName,CS_RESULTS_TABLES_TABLE_NAME ,CFE_TBL_MAX_NAME_LENGTH) == 0)
                {
                    TableHandle = CS_AppData.ResTablesTableHandle;
                    Owned = TRUE;
                }
                if (strncmp (TableTableName,CS_RESULTS_APP_TABLE_NAME ,CFE_TBL_MAX_NAME_LENGTH) == 0)
                {
                    TableHandle = CS_AppData.ResAppTableHandle;
                    Owned = TRUE;
                }
                
            }
            
            /* This is just a real simple test, because all of the exception handling
             has already been done by the Validation routine above */
            NumRegionsInTable++;
            
            ResultsEntry -> State              = DefEntry -> State;
            ResultsEntry -> ComputedYet        = FALSE;
            ResultsEntry -> NumBytesToChecksum = 0;                              /* this is unknown at this time */
            ResultsEntry -> ComparisonValue    = 0;
            ResultsEntry -> ByteOffset         = 0;
            ResultsEntry -> TempChecksumValue  = 0;
            ResultsEntry -> StartAddress       = 0;                              /* this is unknown at this time */
            ResultsEntry -> TblHandle          = TableHandle;
            ResultsEntry -> IsCSOwner          = Owned;
            strncpy(ResultsEntry -> Name, DefEntry -> Name, CFE_TBL_MAX_FULL_NAME_LEN);
        }
        else
        {
            ResultsEntry -> State              = CS_STATE_EMPTY;
            ResultsEntry -> ComputedYet        = FALSE;
            ResultsEntry -> NumBytesToChecksum = 0;
            ResultsEntry -> ComparisonValue    = 0;
            ResultsEntry -> ByteOffset         = 0;
            ResultsEntry -> TempChecksumValue  = 0;
            ResultsEntry -> StartAddress       = 0; 
            ResultsEntry -> TblHandle          = CFE_TBL_BAD_TABLE_HANDLE; 
            ResultsEntry -> IsCSOwner          = FALSE;
            ResultsEntry -> Name[0]            = '\0';
        }
    }
    
    /* Reset the table back to the original checksumming state */

    CS_AppData.TablesCSState = PreviousState;

    if (NumRegionsInTable == 0)
    {
        CFE_EVS_SendEvent (CS_PROCESS_TABLES_NO_ENTRIES_INF_EID,
                           CFE_EVS_INFORMATION,
                           "CS Tables Table: No valid entries in the table");
    }
    return;
}   /* end of CS_ProcessNewTablesDefinitionTable () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS processing new definition tables for Apps                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ProcessNewAppDefinitionTable    (CS_Def_App_Table_Entry_t      * DefinitionTblPtr, 
                                         CS_Res_App_Table_Entry_t      * ResultsTblPtr)
{
    CS_Def_App_Table_Entry_t                  * StartOfDefTable     = NULL;
    CS_Def_App_Table_Entry_t                  * DefEntry            = NULL;
    CS_Res_App_Table_Entry_t                  * StartOfResultsTable = NULL;
    CS_Res_App_Table_Entry_t                  * ResultsEntry        = NULL;
    uint16                                      Loop                = 0;
    uint16                                      NumRegionsInTable   = 0;
    uint16                                      PreviousState       = CS_STATE_EMPTY;
    
    CFE_PSP_MemCpy(&StartOfResultsTable, ResultsTblPtr,    sizeof(StartOfResultsTable));
    CFE_PSP_MemCpy(&StartOfDefTable,     DefinitionTblPtr, sizeof(StartOfDefTable));  
    
    /* We don't want to be doing chekcksums while changing the table out */

    PreviousState = CS_AppData.AppCSState;
    CS_AppData.AppCSState = CS_STATE_DISABLED;
    
    for (Loop = 0; Loop < CS_MAX_NUM_APP_TABLE_ENTRIES; Loop++)
    {
        DefEntry = & (StartOfDefTable [Loop]);
        
        ResultsEntry = & (StartOfResultsTable [Loop]);
        
        if (DefEntry -> State != CS_STATE_EMPTY)
        {
            /* This is just a real simple test, because all of the exception handling
             has already been done by the Validation routine above */
            NumRegionsInTable++;
            
            ResultsEntry -> State              = DefEntry -> State;
            ResultsEntry -> ComputedYet        = FALSE;
            ResultsEntry -> NumBytesToChecksum = 0;       /* this is unknown at this time */
            ResultsEntry -> ComparisonValue    = 0;
            ResultsEntry -> ByteOffset         = 0;
            ResultsEntry -> TempChecksumValue  = 0;
            ResultsEntry -> StartAddress       = 0;       /* this is unknown at this time */
            strncpy(ResultsEntry -> Name, DefEntry -> Name, OS_MAX_API_NAME);
        }
        else
        {
            
            ResultsEntry -> State              = CS_STATE_EMPTY;
            ResultsEntry -> ComputedYet        = FALSE;
            ResultsEntry -> NumBytesToChecksum = 0;
            ResultsEntry -> ComparisonValue    = 0;
            ResultsEntry -> ByteOffset         = 0;
            ResultsEntry -> TempChecksumValue  = 0;
            ResultsEntry -> StartAddress       = 0; 
   
            ResultsEntry -> Name[0]           = '\0';
        }
    }
    
    /* Reset the table back to the original checksumming state */
    
    CS_AppData.AppCSState = PreviousState;
    CS_ResetTablesTblResultEntry(CS_AppData.AppResTablesTblPtr);
    
    if (NumRegionsInTable == 0)
    {
        CFE_EVS_SendEvent (CS_PROCESS_APP_NO_ENTRIES_INF_EID,
                           CFE_EVS_INFORMATION,
                           "CS Apps Table: No valid entries in the table");
    }
    return;
}   /* end of CS_ProcessNewAppsDefinitionTable () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS  function for initializing new tables                        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CS_TableInit (CFE_TBL_Handle_t          * DefinitionTableHandle,
                    CFE_TBL_Handle_t          * ResultsTableHandle,
                    void                      * DefinitionTblPtr,
                    void                      * ResultsTblPtr,
                    uint16                      Table, 
                    char                      * DefinitionTableName,
                    char                      * ResultsTableName,
                    uint16                      NumEntries,
                    char*                       DefinitionTableFileName,
                    void                      * DefaultDefTableAddress,
                    uint16                      SizeofDefinitionTableEntry,
                    uint16                      SizeofResultsTableEntry,
                    CFE_TBL_CallbackFuncPtr_t   CallBackFunction)
{
    int32       Result           = CFE_SUCCESS;
    int32       ResultFromLoad   = OS_FS_ERROR;
    int32       SizeOfTable      = 0;
    boolean     LoadedFromMemory = FALSE;
    boolean     ValidFile        = FALSE;
    int32       Fd               = -1;
    char        TableType[CS_TABLETYPE_NAME_SIZE];
    
    strncpy(TableType, "Undef Tbl", CS_TABLETYPE_NAME_SIZE);   /* Init table type */

    SizeOfTable = NumEntries * SizeofResultsTableEntry;
    
    
    Result = CFE_TBL_Register (ResultsTableHandle,
                               ResultsTableName,
                               SizeOfTable,
                               CFE_TBL_OPT_SNGL_BUFFER | CFE_TBL_OPT_DUMP_ONLY,
                               NULL);
    
    
    if (Result == CFE_SUCCESS)
    {
        Result = CFE_TBL_GetAddress ( ResultsTblPtr,
                                     * ResultsTableHandle);
    }
    
    if (Result == CFE_SUCCESS)
    {
        SizeOfTable = NumEntries * SizeofDefinitionTableEntry;
        
        Result = CFE_TBL_Register (DefinitionTableHandle,
                                   DefinitionTableName,
                                   SizeOfTable,
                                   CFE_TBL_OPT_SNGL_BUFFER | CFE_TBL_OPT_LOAD_DUMP,
                                   CallBackFunction);

        
        
    }
    
    Fd = OS_open(DefinitionTableFileName, OS_READ_ONLY, 0);
    
    if (Fd >= 0)
    {
        ValidFile = TRUE;
        OS_close (Fd);
    }
    
    if ((Result == CFE_SUCCESS) && (ValidFile == TRUE))
    {
       Result = CFE_TBL_Load (* DefinitionTableHandle, 
                               CFE_TBL_SRC_FILE,
                               DefinitionTableFileName);
        ResultFromLoad = Result;
    }
    
    /* if the load from the file fails, load from 
     the default tables in CS */
    if( ResultFromLoad != CFE_SUCCESS)
    {

        Result = CFE_TBL_Load( * DefinitionTableHandle, 
                                CFE_TBL_SRC_ADDRESS,
                                DefaultDefTableAddress);
        LoadedFromMemory = TRUE;
    }
    
    if (Result == CFE_SUCCESS)
    {
        Result = CFE_TBL_GetAddress ( DefinitionTblPtr,
                                      * DefinitionTableHandle);
        
        if ((Result == CFE_TBL_INFO_UPDATED) )
        {
            if (Table == CS_APP_TABLE)
            {
                
                CS_ProcessNewAppDefinitionTable ((CS_Def_App_Table_Entry_t *) DefinitionTblPtr, 
                                                 (CS_Res_App_Table_Entry_t *) ResultsTblPtr);
            }
            else
            {
                if (Table == CS_TABLES_TABLE)
                {
                    CS_ProcessNewTablesDefinitionTable ((CS_Def_Tables_Table_Entry_t *) DefinitionTblPtr, 
                                                        (CS_Res_Tables_Table_Entry_t *) ResultsTblPtr);
                }
                else
                {
                    CS_ProcessNewEepromMemoryDefinitionTable ((CS_Def_EepromMemory_Table_Entry_t *) DefinitionTblPtr, 
                                                              (CS_Res_EepromMemory_Table_Entry_t *) ResultsTblPtr,
                                                              NumEntries,
                                                              Table);
                }
                
            }
            
        }/* end if (Result == CFE_TBL_INFO_UPDATED) || (Result == CFE_SUCCESS) */
    }
    
    if (Result >= CFE_SUCCESS)
    {
        Result = CFE_SUCCESS;
    }
    else
    {
        if( Table == CS_EEPROM_TABLE)
        {
            strncpy(TableType, "Eeprom", CS_TABLETYPE_NAME_SIZE);
        }
        if( Table == CS_MEMORY_TABLE)
        {
            strncpy(TableType, "Memory", CS_TABLETYPE_NAME_SIZE);
        }
        if( Table == CS_TABLES_TABLE)
        {
            strncpy(TableType, "Tables", CS_TABLETYPE_NAME_SIZE);
        }
        if( Table == CS_APP_TABLE)
        {
            strncpy(TableType, "Apps", CS_TABLETYPE_NAME_SIZE);
        }
        
        CFE_EVS_SendEvent (CS_TBL_INIT_ERR_EID,
                           CFE_EVS_ERROR,
                           "CS received error 0x%08X initializing Definition table for %s", 
                           (unsigned int)Result,
                           TableType);
    }
    
    /* If we loaded from file successfully then the states we wish to use have already been set
     * If we loaded from memory then disable the table  */
    if (LoadedFromMemory == TRUE && Result == CFE_SUCCESS)
    {
        switch (Table)
        {
            case CS_EEPROM_TABLE:
                CS_AppData.EepromCSState = CS_STATE_DISABLED;
                break;
            case CS_MEMORY_TABLE:
                CS_AppData.MemoryCSState = CS_STATE_DISABLED;
                break;
            case CS_APP_TABLE:
                CS_AppData.AppCSState    = CS_STATE_DISABLED;
                break;
            case CS_TABLES_TABLE:
                CS_AppData.TablesCSState = CS_STATE_DISABLED;
                break;
            default:
                break;
        }
    }
    return (Result);
    
}   /* end of CS_CheckSum_Definition_Table_Init () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Handles table updates                                        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CS_HandleTableUpdate (void           * DefinitionTblPtr,
                            void           * ResultsTblPtr,
                            CFE_TBL_Handle_t DefinitionTableHandle,
                            CFE_TBL_Handle_t ResultsTableHandle,
                            uint16           Table,
                            uint16           NumEntries)
{
    int32           ReleaseResult1 = CFE_SUCCESS;
    int32           ManageResult1  = CFE_SUCCESS;
    int32           GetResult1     = CFE_SUCCESS;
    int32           ReleaseResult2 = CFE_SUCCESS;
    int32           ManageResult2  = CFE_SUCCESS;
    int32           GetResult2     = CFE_SUCCESS;
    int32           Result         = CFE_SUCCESS;
    int32           Loop           = 0;
    char            TableType[CS_TABLETYPE_NAME_SIZE];

    strncpy(TableType, "Undef Tbl", CS_TABLETYPE_NAME_SIZE);  /* Init table type */
    
    /* Below, there are several values that are returned and assigned, but never evaluated. */
    /* This is done so intentionally, as it helps us with Source-Level debugging this functions. */
    
    /* Release the Table Address.  */ 
    ReleaseResult1 = CFE_TBL_ReleaseAddress (ResultsTableHandle);
    ReleaseResult2 = CFE_TBL_ReleaseAddress (DefinitionTableHandle);
    
    ManageResult1 = CFE_TBL_Manage (ResultsTableHandle);
    ManageResult2 = CFE_TBL_Manage (DefinitionTableHandle);
    
    GetResult1 = CFE_TBL_GetAddress ( ResultsTblPtr,
                                     ResultsTableHandle);
    Result = GetResult1;
    
    if (Result == CFE_SUCCESS)
    {
        GetResult2 = CFE_TBL_GetAddress ( DefinitionTblPtr,
                                          DefinitionTableHandle);
        Result = GetResult2;
    }
    
    if ( (Result == CFE_TBL_INFO_UPDATED))
    {
        if ( Table == CS_TABLES_TABLE)
        {
            /* before we update the results table, we need to release all of the 
             table handles that are in the results table */
            for (Loop = 0; Loop < CS_MAX_NUM_TABLES_TABLE_ENTRIES; Loop++)
            {
                if (CS_AppData.ResTablesTblPtr[Loop].TblHandle != CFE_TBL_BAD_TABLE_HANDLE)
                {
                    
                    if (CS_AppData.ResTablesTblPtr[Loop].IsCSOwner == FALSE)
                    {
                    
                        CFE_TBL_Unregister(CS_AppData.ResTablesTblPtr[Loop].TblHandle);
                    }
                }
            }
            
            CS_ProcessNewTablesDefinitionTable ((CS_Def_Tables_Table_Entry_t *) DefinitionTblPtr, 
                                                (CS_Res_Tables_Table_Entry_t *) ResultsTblPtr);
        }
        else
        {
            
            if (Table == CS_APP_TABLE)
            {
                CS_ProcessNewAppDefinitionTable ((CS_Def_App_Table_Entry_t *) DefinitionTblPtr, 
                                                 (CS_Res_App_Table_Entry_t *) ResultsTblPtr);
            }
            else
            {
                CS_ProcessNewEepromMemoryDefinitionTable ((CS_Def_EepromMemory_Table_Entry_t *) DefinitionTblPtr, 
                                                          (CS_Res_EepromMemory_Table_Entry_t *) ResultsTblPtr,
                                                          NumEntries,
                                                          Table);
            }
        }
        
        Result = CFE_SUCCESS;
    }
    else
    {
        if (Result < CFE_SUCCESS)
        {
            if( Table == CS_EEPROM_TABLE)
            {
                strncpy(TableType, "Eeprom", CS_TABLETYPE_NAME_SIZE);
            }
            if( Table == CS_MEMORY_TABLE)
            {
                strncpy(TableType, "Memory", CS_TABLETYPE_NAME_SIZE);
            }
            if( Table == CS_TABLES_TABLE)
            {
                strncpy(TableType, "Table", CS_TABLETYPE_NAME_SIZE);
            }
            if( Table == CS_APP_TABLE)
            {
                strncpy(TableType, "App", CS_TABLETYPE_NAME_SIZE);
            }
            
            /* There was a problem somewhere, generate an event */
            CFE_EVS_SendEvent (CS_TBL_UPDATE_ERR_EID,
                               CFE_EVS_ERROR,
                               "CS had problems updating table. Release:0x%08X Manage:0x%08X Get:0x%08X for table %s",
                               (unsigned int)ReleaseResult2,
                               (unsigned int)ManageResult2,
                               (unsigned int)GetResult2,
                               TableType);
        }
    }
    return (Result);
}   /* end CS_HandleTableUpdate */


/************************/
/*  End of File Comment */
/************************/
