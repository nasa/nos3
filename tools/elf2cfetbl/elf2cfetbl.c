/*
** $Id: elf2cfetbl.c 1.9 2014/05/22 17:46:02GMT-05:00 sstrege Exp  $
**
**      Copyright (c) 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement. 
**
**  Purpose:  
**    This file implements the ELF file to Standard cFE Table file format tool
**
**  Notes:
** 
**  Modification History:
**
** $Log: elf2cfetbl.c  $
** Revision 1.9 2014/05/22 17:46:02GMT-05:00 sstrege 
** Replaced MAX_FILENAME_SIZE with OS_MAX_FILE_NAME
** Revision 1.8 2012/01/13 13:06:12EST acudmore 
** Updated license text to reflect open source
** Revision 1.7 2011/12/15 17:32:57EST lwalling 
** Add cmd line option for processor ID as 4 ASCII characters
** Revision 1.6 2011/12/15 17:20:59EST lwalling 
** Add cmd line option for spacecraft ID as 4 ASCII characters
** Revision 1.5 2010/11/16 00:17:08EST dkobe 
** Added command line option '-T' to enable insertion of source file creation 
** time into standard cFE File Header.
** Revision 1.4 2010/11/15 23:38:43EST dkobe 
** Added byte alignment command line option ("-n")
** Revision 1.3 2010/10/14 16:02:21EDT jmdagost 
** Corrected malloc size, added Verbose mode statements for symbol names.
** Revision 1.2 2008/07/31 16:01:39EDT dkobe 
** Updated to include object file modification time in header of table image file output
** Revision 1.1 2008/04/17 08:08:48EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/elf2cfetbl/project.pj
** Revision 1.1.1.4 2007/07/10 09:46:17EDT David Kobe (dlkobe) 
** Version 2.1: Added ability to recognize tables containing all zeros and process them correctly
**
*/

/*
** Required header files.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>
#include "ELF_Structures.h"
#include "cfe_tbl_internal.h"
#include "cfe_tbl_filedef.h"
#include "osconfig.h"

#define MAX_SECTION_HDR_NAME_LEN   (128)
#define TBL_DEF_SYMBOL_NAME "CFE_TBL_FileDef"
#define SUCCESS (0)
#define FAILED  (1)

/* macro to construct 32 bit value from 4 chars */
#define U32FROM4CHARS( _C1, _C2, _C3, _C4 ) \
 ( (uint32)(_C1) << 24 | \
   (uint32)(_C2) << 16 | \
   (uint32)(_C3) << 8 | \
   (uint32)(_C4) )

typedef struct
{
    int32        Value;
    char         String[50];
} ElfStrMap;

/**
*    Function Prototypes
*/
int32 ProcessCmdLineOptions(int argc, char *argv[]);
int32 GetSrcFilename(void);
int32 GetDstFilename(void);
int32 OpenSrcFile(void);
int32 OpenDstFile(void);
int32 GetElfHeader(void);
void  SwapElfHeader(void);
int32 GetSectionHeader(int32 SectionIndex, Elf32_Shdr *SectionHeader);
void  SwapSectionHeader(Elf32_Shdr *SectionHeader);
int32 GetSymbol(int32 SymbolIndex, Elf32_Sym *Symbol);
void  SwapSymbol(Elf32_Sym *Symbol);
int32 GetStringFromMap(char *Result, ElfStrMap *Map, int32 Key);
void  SwapUInt16(uint16 *ValueToSwap);
void  SwapUInt32(uint32 *ValueToSwap);
int32 AllocateSectionHeaders(void);
void  DeallocateSectionHeaders(void);
int32 AllocateSymbols(void);
void  DeallocateSymbols(void);
void  FreeMemoryAllocations(void);
int32 GetTblDefInfo(void);
int32 OutputDataToTargetFile(void);
void  OutputVersionInfo(void);
void  OutputHelpInfo(void);
int32 LocateAndReadUserObject(void);

/**
*    Global Variables
*/
char SrcFilename[OS_MAX_FILE_NAME+3]={""};
char DstFilename[OS_MAX_FILE_NAME+3]={""};
char TableName[38]={""};
char Description[32]={""};
char LineOfText[300]={""};

boolean Verbose=FALSE;
boolean ReportVersion=FALSE;
boolean OutputHelp=FALSE;
boolean ByteSwapRequired=FALSE;
boolean ScIDSpecified=FALSE;
boolean ProcIDSpecified=FALSE;
boolean AppIDSpecified=FALSE;
boolean ScEpochSpecified=FALSE;
boolean FileEpochSpecified=FALSE;
boolean TableNameOverride=FALSE;
boolean DescriptionOverride=FALSE;
boolean ThisMachineIsLittleEndian=TRUE;
boolean TargetMachineIsLittleEndian=TRUE;
boolean ByteAlignFileHeaders=TRUE;
boolean EnableTimeTagInHeader=FALSE;

boolean TableDataIsAllZeros=FALSE;

FILE *SrcFileDesc;
FILE *DstFileDesc;

CFE_FS_Header_t     FileHeader;
CFE_TBL_File_Hdr_t  TableHeader;

Elf32_Ehdr ElfHeader;
Elf32_Shdr **SectionHeaderPtrs = NULL;
Elf32_Shdr SectionHeaderStringTable = {0, 0, 0, NULL, 0, 0, 0, 0, 0, 0 };
int32 SectionHeaderStringTableDataOffset = 0;
char  **SectionNamePtrs = NULL;

struct stat SrcFileStats;

int32 StringTableDataOffset = 0;
int32 SymbolTableDataOffset = 0;
int32 NumSymbols = 0;
int32 SymbolTableEntrySize = 0;
Elf32_Sym **SymbolPtrs = NULL;
char **SymbolNames;
int32 TblDefSymbolIndex = -1;
CFE_TBL_FileDef_t TblFileDef;
int32 UserObjSymbolIndex = -1;
uint32 SpacecraftID=0;
uint32 ProcessorID=0;
uint32 ApplicationID=0;
time_t EpochTime=0;

typedef struct {
    uint32 Year;
    uint32 Month;
    uint32 Day;
    uint32 Hour;
    uint32 Minute;
    uint32 Second;
} SpecifiedEpoch_t;

SpecifiedEpoch_t ScEpoch   = {1970,1,1,0,0,0};
SpecifiedEpoch_t FileEpoch = {1970,1,1,0,0,0};
time_t EpochDelta;
time_t SrcFileTimeInScEpoch;

/**
*    ELF Characteristic Maps
*/
ElfStrMap e_type_Map[] = 
{
    { ET_NONE, "ET_NONE (0)" },
    { ET_REL,  "ET_REL (1)"  }, 
    { ET_EXEC, "ET_EXEC (2)" },
    { ET_DYN,  "ET_DYN (3)"  },
    { ET_CORE, "ET_CORE (4)" },
    { 0, "* Unknown Elf File Type (%d) *" },
};

ElfStrMap e_machine_Map[] =
{
     { EM_NONE        , "EM_NONE         ( 0)" }, 
     { EM_M32         , "EM_M32          ( 1)" }, 
     { EM_SPARC       , "EM_SPARC        ( 2)" }, 
     { EM_386         , "EM_386          ( 3)" }, 
     { EM_68K         , "EM_68K          ( 4)" }, 
     { EM_88K         , "EM_88K          ( 5)" }, 
     { EM_860         , "EM_860          ( 7)" }, 
     { EM_MIPS        , "EM_MIPS         ( 8)" }, 
     { EM_S370        , "EM_S370         ( 9)" }, 
     { EM_MIPS_RS3_LE , "EM_MIPS_RS3_LE  (10)" }, 
     { EM_PARISC      , "EM_PARISC       (15)" }, 
     { EM_VPP500      , "EM_VPP500       (17)" }, 
     { EM_SPARC32PLUS , "EM_SPARC32PLUS  (18)" }, 
     { EM_960         , "EM_960          (19)" }, 
     { EM_PPC         , "EM_PPC          (20)" }, 
     { EM_PPC64       , "EM_PPC64        (21)" }, 
     { EM_V800        , "EM_V800         (36)" }, 
     { EM_FR20        , "EM_FR20         (37)" }, 
     { EM_RH32        , "EM_RH32         (38)" }, 
     { EM_RCE         , "EM_RCE          (39)" }, 
     { EM_ARM         , "EM_ARM          (40)" }, 
     { EM_ALPHA       , "EM_ALPHA        (41)" }, 
     { EM_SH          , "EM_SH           (42)" }, 
     { EM_SPARCV9     , "EM_SPARCV9      (43)" }, 
     { EM_TRICORE     , "EM_TRICORE      (44)" }, 
     { EM_ARC         , "EM_ARC          (45)" }, 
     { EM_H8_300      , "EM_H8_300       (46)" }, 
     { EM_H8_300H     , "EM_H8_300H      (47)" }, 
     { EM_H8S         , "EM_H8S          (48)" }, 
     { EM_H8_500      , "EM_H8_500       (49)" }, 
     { EM_IA_64       , "EM_IA_64        (50)" }, 
     { EM_MIPS_X      , "EM_MIPS_X       (51)" }, 
     { EM_COLDFIRE    , "EM_COLDFIRE     (52)" }, 
     { EM_68HC12      , "EM_68HC12       (53)" }, 
     { EM_MMA         , "EM_MMA          (54)" }, 
     { EM_PCP         , "EM_PCP          (55)" }, 
     { EM_NCPU        , "EM_NCPU         (56)" }, 
     { EM_NDR1        , "EM_NDR1         (57)" }, 
     { EM_STARCORE    , "EM_STARCORE     (58)" }, 
     { EM_ME16        , "EM_ME16         (59)" }, 
     { EM_ST100       , "EM_ST100        (60)" }, 
     { EM_TINYJ       , "EM_TINYJ        (61)" }, 
     { EM_FX66        , "EM_FX66         (66)" }, 
     { EM_ST9PLUS     , "EM_ST9PLUS      (67)" }, 
     { EM_ST7         , "EM_ST7          (68)" }, 
     { EM_68HC16      , "EM_68HC16       (69)" }, 
     { EM_68HC11      , "EM_68HC11       (70)" }, 
     { EM_68HC08      , "EM_68HC08       (71)" }, 
     { EM_68HC05      , "EM_68HC05       (72)" }, 
     { EM_SVX         , "EM_SVX          (73)" }, 
     { EM_ST19        , "EM_ST19         (74)" }, 
     { EM_VAX         , "EM_VAX          (75)" }, 
     { EM_CRIS        , "EM_CRIS         (76)" }, 
     { EM_JAVELIN     , "EM_JAVELIN      (77)" }, 
     { EM_FIREPATH    , "EM_FIREPATH     (78)" }, 
     { EM_ZSP         , "EM_ZSP          (79)" }, 
     { EM_MMIX        , "EM_MMIX         (80)" }, 
     { EM_HUANY       , "EM_HUANY        (81)" }, 
     { EM_PRISM       , "EM_PRISM        (82)" },
     { 0              , "* Unknown Machine Type (%d) *" }, 
};

/**
*
*/

int main(int argc, char *argv[])
{
    int32  Status = SUCCESS;
    int32  i=0;

    Status = ProcessCmdLineOptions(argc, argv);
    if (Status != SUCCESS) return Status;
    
    if (ReportVersion) OutputVersionInfo();

    Status = GetSrcFilename();
    if (OutputHelp) OutputHelpInfo();
    if (Status != SUCCESS) return Status;
    
    Status = OpenSrcFile();
    if (Status != SUCCESS) return Status;
    
    Status = GetElfHeader();
    if (Status != SUCCESS) return Status;
    
    /* Get the string section header first */
    Status = GetSectionHeader(ElfHeader.e_shstrndx, &SectionHeaderStringTable);
    if (Status != SUCCESS) return Status;
    
    SectionHeaderStringTableDataOffset = SectionHeaderStringTable.sh_offset;
    
    /* Allocate memory for all of the ELF object file section headers */
    Status = AllocateSectionHeaders();
    if (Status != SUCCESS)
    {
        FreeMemoryAllocations();
        return Status;
    }
    
    /* Read in each section header from input file */
    for (i=0; i<ElfHeader.e_shnum; i++)
    {
        Status = GetSectionHeader(i, SectionHeaderPtrs[i]);
        if (Status != SUCCESS)
        {
            FreeMemoryAllocations();
            return Status;
        }
    }
    
    /* Allocate memory for all of the symbol table entries */
    Status = AllocateSymbols();
    if (Status != SUCCESS)
    {
        FreeMemoryAllocations();
        return Status;
    }
    
    /* Read in each symbol table entry */
    for (i=0; i<NumSymbols; i++)
    {
        Status = GetSymbol(i, SymbolPtrs[i]);
        if (Status != SUCCESS)
        {
            FreeMemoryAllocations();
            return Status;
        }
    }
    
    if (TblDefSymbolIndex == -1)
    {
        printf("Error! Unable to locate '%s' object in '%s'.\n", TBL_DEF_SYMBOL_NAME, SrcFilename);
        FreeMemoryAllocations();
        return Status;
    }
    
    /* Read in the definition of the table file */
    Status = GetTblDefInfo();
    if (Status != SUCCESS)
    {
        FreeMemoryAllocations();
        return Status;
    }
    
    Status = GetDstFilename();
    if (Status != SUCCESS) return Status;
    
    Status = OpenDstFile();
    if (Status != SUCCESS) return Status;
    
    Status = LocateAndReadUserObject();
    if (Status != SUCCESS)
    {
        FreeMemoryAllocations();
        return Status;
    }
    
    Status = OutputDataToTargetFile();

    FreeMemoryAllocations();
    
    return SUCCESS;
}

/**
*
*/

int32 AllocateSectionHeaders(void)
{
    int32 Status = SUCCESS;
    int32 i=0;
    
    if (ElfHeader.e_shnum == 0)
    {
        printf("Error! Failed to locate any Section Headers in '%s'!\n", SrcFilename);
        Status = FAILED;
    }
    else
    {
        SectionHeaderPtrs = (Elf32_Shdr **)malloc(sizeof(Elf32_Shdr *)*ElfHeader.e_shnum);
        if (SectionHeaderPtrs == NULL)
        {
            printf("Error! Insufficient memory for number of Sections in '%s'!\n", SrcFilename);
            Status = FAILED;
        }
        
        SectionNamePtrs = (char **)malloc(sizeof(char *)*ElfHeader.e_shnum);
        if (SectionNamePtrs == NULL)
        {
            printf("Error! Insufficient memory for number of Sections in '%s'!\n", SrcFilename);
            Status = FAILED;
        }
    
        if (Status == SUCCESS)
        {
            /* Initialize all of the pointers to NULL */
            for (i=0; i<ElfHeader.e_shnum; i++)
            {
                SectionHeaderPtrs[i] = NULL;
                SectionNamePtrs[i] = NULL;
            }
        
            /* Allocate memory for each header */
            for (i=0; i<ElfHeader.e_shnum; i++)
            {
                SectionHeaderPtrs[i] = (Elf32_Shdr *)malloc(sizeof(Elf32_Shdr));
                if (SectionHeaderPtrs[i] == NULL) 
                {
                    printf("Error! Insufficient memory to store Section Headers\n");
                    Status = FAILED;
                }
                
                SectionNamePtrs[i] = (char *)malloc(MAX_SECTION_HDR_NAME_LEN);
                if (SectionNamePtrs[i] == NULL) 
                {
                    printf("Error! Insufficient memory to store Section Names\n");
                    Status = FAILED;
                }
            }
        }
    }
    
    return Status;
}

/**
*
*/

void DeallocateSectionHeaders(void)
{
    int32 i=0;
    
    if (SectionHeaderPtrs != NULL)
    {
        while ((i<ElfHeader.e_shnum) && (SectionHeaderPtrs[i] != NULL))
        {
            free(SectionHeaderPtrs[i]);
            i++;
        }
        free(SectionHeaderPtrs);
    }
    
    i=0;
    
    if (SectionNamePtrs != NULL)
    {
        while ((i<ElfHeader.e_shnum) && (SectionNamePtrs[i] != NULL))
        {
            free(SectionNamePtrs[i]);
            i++;
        }
        free(SectionNamePtrs);
    }
}

/**
*
*/

int32 AllocateSymbols(void)
{
    int32 Status = SUCCESS;
    int32 i=0;
    
    if (NumSymbols == 0)
    {
        printf("Error! Failed to locate any Symbols in '%s'!\n", SrcFilename);
        Status = FAILED;
    }
    else
    {
        SymbolPtrs = malloc(sizeof(Elf32_Sym *)*NumSymbols);
    
        if (SymbolPtrs == NULL)
        {
            printf("Error! Insufficient memory for number of Symbols in '%s'!\n", SrcFilename);
            Status = FAILED;
        }
    
        SymbolNames = malloc(sizeof(char *)*NumSymbols);
        if (SymbolNames == NULL)
        {
            printf("Error! Insufficient memory for number of Symbols in '%s'!\n", SrcFilename);
            Status = FAILED;
        }
        
        for (i=0; i<NumSymbols; i++)
        {
            SymbolPtrs[i] = NULL;
            SymbolNames[i] = NULL;
        }
        
        if (Status == SUCCESS)
        {
            /* Allocate memory for each symbol */
            for (i=0; i<NumSymbols; i++)
            {
                SymbolPtrs[i] = (Elf32_Sym *)malloc(sizeof(Elf32_Sym));
                if (SymbolPtrs[i] == NULL) 
                {
                    printf("Error! Insufficient memory to store Symbol Headers\n");
                    Status = FAILED;
                }
            }
        }
    }
    
    return Status;
}

/**
*
*/

void DeallocateSymbols(void)
{
    int32 i=0;
    
    if (SymbolPtrs != NULL)
    {
        while ((i<NumSymbols) && (SymbolPtrs[i] != NULL))
        {
            free(SymbolPtrs[i]);
            
            if (SymbolNames[i] != NULL)
            {
                free(SymbolNames[i]);
            }
            
            i++;
        }
        free(SymbolPtrs);
    }
}

/**
*
*/

void FreeMemoryAllocations(void)
{
    DeallocateSymbols();
    DeallocateSectionHeaders();
    fclose(SrcFileDesc);
    fclose(DstFileDesc);
}

/**
*
*/

int32 ProcessCmdLineOptions(int ArgumentCount, char *Arguments[])
{
    int32      Status = SUCCESS;
    boolean    InputFileSpecified = FALSE;
    boolean    OutputFileSpecified = FALSE;
    int16      i=1;
    char      *EndPtr;
    uint32     MaxDay;
    struct tm  FileEpochTm;
    struct tm  ScEpochTm;
    time_t     FileEpochInSecs;
    time_t     ScEpochInSecs;

    while ((i<ArgumentCount) && (Status == SUCCESS))
    {
        if ((Arguments[i][0] == '-') && (Arguments[i][1] == 't'))
        {
            // Extract the Table Name Override
            strncpy(TableName, &Arguments[i][2], 38);
            TableNameOverride = TRUE;
        }
        else if ((Arguments[i][0] == '-') && (Arguments[i][1] == 'd'))
        {
            // Extract the Description Override
            strncpy(Description, &Arguments[i][2], 32);
            DescriptionOverride = TRUE;
        }
        else if ((Arguments[i][0] == '-') && (Arguments[i][1] == 's'))
        {
            SpacecraftID = strtoul(&Arguments[i][2], &EndPtr, 0);
            if (EndPtr != &Arguments[i][2])
            {
                ScIDSpecified = TRUE;
            }
            else
            {
                printf("Error!, Spacecraft ID of '%s' cannot be interpreted as an integer.\n", &Arguments[i][2]);
                Status = FALSE;
            }
        }
        else if ((Arguments[i][0] == '-') && (Arguments[i][1] == 'S'))
        {
            if (strlen(&Arguments[i][2]) == 4)
            {
                SpacecraftID = U32FROM4CHARS(Arguments[i][2],Arguments[i][3],
                                             Arguments[i][4],Arguments[i][5]); 
                ScIDSpecified = TRUE;
            }
            else
            {
                printf("Error!, Spacecraft ID of '%s' does not have exactly 4 characters.\n", &Arguments[i][2]);
                Status = FALSE;
            }
        }
        else if ((Arguments[i][0] == '-') && (Arguments[i][1] == 'a'))
        {
            ApplicationID = strtoul(&Arguments[i][2], &EndPtr, 0);
            if (EndPtr != &Arguments[i][2])
            {
                AppIDSpecified = TRUE;
            }
            else
            {
                printf("Error!, Application ID of '%s' cannot be interpreted as an integer.\n", &Arguments[i][2]);
                Status = FALSE;
            }
        }
        else if ((Arguments[i][0] == '-') && (Arguments[i][1] == 'p'))
        {
            ProcIDSpecified = TRUE;
            ProcessorID = strtoul(&Arguments[i][2], &EndPtr, 0);
            if (EndPtr != &Arguments[i][2])
            {
                ProcIDSpecified = TRUE;
            }
            else
            {
                printf("Error!, Processor ID of '%s' cannot be interpreted as an integer.\n", &Arguments[i][2]);
                Status = FALSE;
            }
        }
        else if ((Arguments[i][0] == '-') && (Arguments[i][1] == 'P'))
        {
            if (strlen(&Arguments[i][2]) == 4)
            {
                ProcessorID = U32FROM4CHARS(Arguments[i][2],Arguments[i][3],
                                            Arguments[i][4],Arguments[i][5]); 
                ProcIDSpecified = TRUE;
            }
            else
            {
                printf("Error!, Processor ID of '%s' does not have exactly 4 characters.\n", &Arguments[i][2]);
                Status = FALSE;
            }
        }
        else if ((Arguments[i][0] == '-') && (Arguments[i][1] == 'h'))
        {
            OutputHelp = TRUE;
        }
        else if ((Arguments[i][0] == '-') && (Arguments[i][1] == 'v'))
        {
            Verbose = TRUE;
        }
        else if ((Arguments[i][0] == '-') && (Arguments[i][1] == 'V'))
        {
            ReportVersion = TRUE;
        }
        else if ((Arguments[i][0] == '-') && (Arguments[i][1] == 'n'))
        {
            ByteAlignFileHeaders = FALSE;
        }
        else if ((Arguments[i][0] == '-') && (Arguments[i][1] == 'T'))
        {
            EnableTimeTagInHeader = TRUE;
        }
        else if ((Arguments[i][0] == '-') && (Arguments[i][1] == 'e'))
        {
            ScEpoch.Year = strtoul(&Arguments[i][2], &EndPtr, 0);
            if (EndPtr != &Arguments[i][6])
            {
                fprintf(stderr, "Error! Spacecraft Epoch Year is not of the form 'YYYY:'\n");
                Status = FALSE;
            }
            else
            {
                ScEpoch.Month = strtoul(&Arguments[i][7], &EndPtr, 0);
                if ((EndPtr != &Arguments[i][9]) || (ScEpoch.Month == 0) || (ScEpoch.Month > 12))
                {
                    fprintf(stderr, "Error! Spacecraft Epoch Month is not of the form 'MM:' where MM is in the range of 1-12\n");
                    Status = FALSE;
                }
                else
                {
                    MaxDay = 31;
                    if ((ScEpoch.Month == 4) || (ScEpoch.Month == 6) || (ScEpoch.Month == 9) || (ScEpoch.Month == 11))
                    {
                        MaxDay = 30;
                    }
                    else if (ScEpoch.Month == 2)
                    {
                        if ((ScEpoch.Year % 4) == 0)
                        {
                            if ((ScEpoch.Year % 100) == 0)
                            {
                                if ((ScEpoch.Year % 400) == 0)
                                {
                                    MaxDay = 29;
                                }
                                else
                                {
                                    MaxDay = 28;
                                }
                            }
                            else
                            {
                                MaxDay = 29;
                            }
                        }
                        else
                        {
                            MaxDay = 28;
                        }
                    }
                    ScEpoch.Day = strtoul(&Arguments[i][10], &EndPtr, 0);
                    if ((EndPtr != &Arguments[i][12]) || 
                        (ScEpoch.Day == 0) || (ScEpoch.Day > MaxDay))
                    {
                        fprintf(stderr, "Error! Spacecraft Epoch Day is not of the form 'DD:' where DD is in the range of 1-%d\n", MaxDay);
                        Status = FALSE;
                    }
                    else
                    {
                        ScEpoch.Hour = strtoul(&Arguments[i][13], &EndPtr, 0);
                        if ((EndPtr != &Arguments[i][15]) || (ScEpoch.Hour > 23))
                        {
                            fprintf(stderr, "Error! Spacecraft Epoch Hour is not of the form 'hh:' where hh is in the range of 0-23\n");
                            Status = FALSE;
                        }
                        else
                        {
                            ScEpoch.Minute = strtoul(&Arguments[i][16], &EndPtr, 0);
                            if ((EndPtr != &Arguments[i][18]) || (ScEpoch.Minute > 59))
                            {
                                fprintf(stderr, "Error! Spacecraft Epoch Minute is not of the form 'mm:' where mm is in the range of 0-59\n");
                                Status = FALSE;
                            }
                            else
                            {
                                ScEpoch.Second = strtoul(&Arguments[i][19], &EndPtr, 0);
                                if ((EndPtr != &Arguments[i][21]) || (ScEpoch.Second > 59))
                                {
                                    fprintf(stderr, "Error! Spacecraft Epoch Second is not of the form 'ss' where ss is in the range of 0-59\n");
                                    Status = FALSE;
                                }
                                else
                                {
                                    ScEpochSpecified = TRUE;
                                }
                            }
                        }
                    }
                }
            }
        }
        else if ((Arguments[i][0] == '-') && (Arguments[i][1] == 'f'))
        {
            FileEpoch.Year = strtoul(&Arguments[i][2], &EndPtr, 0);
            if (EndPtr != &Arguments[i][6])
            {
                fprintf(stderr, "Error! File Epoch Year is not of the form 'YYYY:'\n");
                Status = FALSE;
            }
            else
            {
                FileEpoch.Month = strtoul(&Arguments[i][7], &EndPtr, 0);
                if ((EndPtr != &Arguments[i][9]) || (FileEpoch.Month == 0) || (FileEpoch.Month > 12))
                {
                    fprintf(stderr, "Error! File Epoch Month is not of the form 'MM:' where MM is in the range of 1-12\n");
                    Status = FALSE;
                }
                else
                {
                    MaxDay = 31;
                    if ((FileEpoch.Month == 4) || (FileEpoch.Month == 6) || (FileEpoch.Month == 9) || (FileEpoch.Month == 11))
                    {
                        MaxDay = 30;
                    }
                    else if (FileEpoch.Month == 2)
                    {
                        if ((FileEpoch.Year % 4) == 0)
                        {
                            if ((FileEpoch.Year % 100) == 0)
                            {
                                if ((FileEpoch.Year % 400) == 0)
                                {
                                    MaxDay = 29;
                                }
                                else
                                {
                                    MaxDay = 28;
                                }
                            }
                            else
                            {
                                MaxDay = 29;
                            }
                        }
                        else
                        {
                            MaxDay = 28;
                        }
                    }
                    FileEpoch.Day = strtoul(&Arguments[i][10], &EndPtr, 0);
                    if ((EndPtr != &Arguments[i][12]) || (FileEpoch.Day == 0) || (FileEpoch.Day > MaxDay))
                    {
                        fprintf(stderr, "Error! File Epoch Day is not of the form 'DD:' where DD is in the range of 1-%d\n", MaxDay);
                        Status = FALSE;
                    }
                    else
                    {
                        FileEpoch.Hour = strtoul(&Arguments[i][13], &EndPtr, 0);
                        if ((EndPtr != &Arguments[i][15]) || (FileEpoch.Hour > 23))
                        {
                            fprintf(stderr, "Error! File Epoch Hour is not of the form 'hh:' where hh is in the range of 0-23\n");
                            Status = FALSE;
                        }
                        else
                        {
                            FileEpoch.Minute = strtoul(&Arguments[i][16], &EndPtr, 0);
                            if ((EndPtr != &Arguments[i][18]) || (FileEpoch.Minute > 59))
                            {
                                fprintf(stderr, "Error! File Epoch Minute is not of the form 'mm:' where mm is in the range of 0-59\n");
                                Status = FALSE;
                            }
                            else
                            {
                                FileEpoch.Second = strtoul(&Arguments[i][19], &EndPtr, 0);
                                if ((EndPtr != &Arguments[i][21]) || (FileEpoch.Second > 59))
                                {
                                    fprintf(stderr, "Error! File Epoch Second is not of the form 'ss' where ss is in the range of 0-59\n");
                                    Status = FALSE;
                                }
                                else
                                {
                                    FileEpochSpecified = TRUE;
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (!InputFileSpecified)
        {
            strncpy(SrcFilename, Arguments[i], OS_MAX_FILE_NAME);
            InputFileSpecified = TRUE;
        }
        else if (!OutputFileSpecified)
        {
            strncpy(DstFilename, Arguments[i], OS_MAX_FILE_NAME);
            OutputFileSpecified = TRUE;
        }
        else
        {
            printf("\nError! Unknown Command Line Option '%s'\n", Arguments[i]);
            Status = FAILED;
        }
        i++;
    }
    FileEpochTm.tm_sec  = FileEpoch.Second;
    FileEpochTm.tm_min  = FileEpoch.Minute;
    FileEpochTm.tm_hour = FileEpoch.Hour;
    FileEpochTm.tm_mday = FileEpoch.Day;
    FileEpochTm.tm_mon  = FileEpoch.Month - 1;
    FileEpochTm.tm_year = FileEpoch.Year - 1900;
    FileEpochTm.tm_isdst = -1;
    
    FileEpochInSecs = mktime(&FileEpochTm);
    
    ScEpochTm.tm_sec  = ScEpoch.Second;
    ScEpochTm.tm_min  = ScEpoch.Minute;
    ScEpochTm.tm_hour = ScEpoch.Hour;
    ScEpochTm.tm_mday = ScEpoch.Day;
    ScEpochTm.tm_mon  = ScEpoch.Month - 1;
    ScEpochTm.tm_year = ScEpoch.Year - 1900;
    ScEpochTm.tm_isdst = -1;
    
    ScEpochInSecs = mktime(&ScEpochTm);

    EpochDelta = FileEpochInSecs - ScEpochInSecs;
    
    return Status;
}

/**
*
*/

void OutputVersionInfo(void)
{
    printf("\nElf Object File to cFE Table Image File Conversion Tool (v3.0)-(%s %s)\nCompatible with cFE Version 4.x and higher\n\n",__DATE__, __TIME__);
}

/**
*
*/

void OutputHelpInfo(void)
{
    printf("elf2cfetbl [-tTblName] [-d\"Description\"] [-h] [-v] [-V] [-s#] [-p#] [-n] \n");
    printf("           [-T] [-eYYYY:MM:DD:hh:mm:ss] [-fYYYY:MM:DD:hh:mm:ss] SrcFilename [DestDirectory]\n");
    printf("   where:\n");
    printf("   -tTblName             replaces the table name specified in the object file with 'TblName'\n");
    printf("   -d\"Description\"       replaces the description specified in the object file with 'Description'\n");
    printf("   -h                    produces this output\n");
    printf("   -v                    produces verbose output showing the breakdown of the object file in detail\n");
    printf("   -V                    shows the version of this utility\n");
    printf("   -s#                   specifies a Spacecraft ID to be put into file header.\n");
    printf("                         # can be specified as decimal, octal (starting with a zero), or hex (starting with '0x')\n");
    printf("   -Scccc                specifies a Spacecraft ID as a 4 byte string to be put into the table file file header.\n");
    printf("                         cccc represents the 4 ASCII characters that will be encoded into the 32 bit Spacecaft ID field.\n");
    printf("                              examples: -SMMS1 or -SQQ#2\n");
    printf("   -p#                   specifies a Processor ID to be put into file header.\n");
    printf("                         # can be specified as decimal, octal (starting with a zero), or hex (starting with '0x')\n");
    printf("   -Pcccc                specifies a Processor ID as a 4 byte string to be put into the table file header.\n");
    printf("                         cccc represents the 4 ASCII characters that will be encoded into the 32 bit Processor ID field.\n");
    printf("                              examples: -PMMS1 or -PQQ#2\n");
    printf("   -a#                   specifies an Application ID to be put into file header.\n");
    printf("                         # can be specified as decimal, octal (starting with a zero), or hex (starting with '0x')\n");
    printf("   -n                    specifies that output should NOT byte align FS header and secondary table header to nearest\n");
    printf("                         4-byte boundary. The default assumes a 4-byte alignment on both structures.\n");
    printf("   -T                    enables insertion of the SrcFilename's file creation time into the standard cFE File Header.\n");
    printf("                         This option must be specified for either the '-e' and/or '-f' options below to have any effect.\n");
    printf("                         By default, the time tag fields are set to zero.\n");
    printf("   -eYYYY:MM:DD:hh:mm:ss specifies the spacecraft epoch time.  The SrcFilename's file creation time will be converted to\n");
    printf("                         seconds since the specified epoch time and stored in the standard cFE File Header.\n");
    printf("                         where:   YYYY=year, MM=month (01-12), DD=day (01-31), \n");
    printf("                                  hh=hour (00-23), mm=minute (00-59), ss=seconds (00-59)\n");
    printf("                         If no epoch is specified, the default epoch is either 1970:01:01:00:00:00 or the epoch specified\n");
    printf("                         by the user using the '-f' option described below\n");
    printf("                         This option requires the '-T' option, defined above, to be specified to have any effect\n");
    printf("   -fYYYY:MM:DD:hh:mm:ss specifies the file system epoch time.  The SrcFilename's file creation time is obtained from the\n");
    printf("                         file system as seconds since an epoch.  On most systems the file system epoch is defined as\n");
    printf("                         1970:01:01:00:00:00.  If the user is running this application on a machine with a different epoch,\n");
    printf("                         then the file system epoch should be defined with this option.\n");
    printf("                         where:   YYYY=year, MM=month (01-12), DD=day (01-31), \n");
    printf("                                  hh=hour (00-23), mm=minute (00-59), ss=seconds (00-59)\n");
    printf("                         If no epoch is specified, the default epoch is 1970:01:01:00:00:00\n");
    printf("                         This option requires the '-T' option, defined above, to be specified to have any effect\n");
    printf("   SrcFilename           specifies the object file to be converted\n");
    printf("   DestDirectory         specifies the directory in which the cFE Table Image file is to be created.\n");
    printf("                         If a directory is not specified './' is assumed.\n");
    printf("\n");
    printf("EXAMPLES:\n");
    printf("   elf2cfetbl MyObjectFile ../../TblDefaultImgDir/\n");
    printf("   elf2cfetbl -s12 -p0x0D -a016 -e2000:01:01:00:00:00 MyObjectFile ../../TblDefaultImgDir/\n");
    printf("\n");
    printf("NOTE: The name of the target file is specified within the source file as part of the CFE_TBL_FILEDEF macro.\n");
    printf("      If the macro has not been included in the source file, the utility will fail to convert the object file.\n");
}

/**
*
*/

int32 GetSrcFilename(void)
{
    int32 Status = SUCCESS;

    if (strlen(SrcFilename) == 0)
    {
        OutputHelp = TRUE;
        Status = FAILED;
    }

    return Status;
}

/**
*
*/

int32 GetDstFilename(void)
{
    int32 Status = SUCCESS;

    if (strlen(DstFilename) == 0)
    {
        strcpy(DstFilename, "./");
    }
    
    strcat(DstFilename, TblFileDef.TgtFilename);
    
    if (Verbose) printf("Target Filename: %s\n", DstFilename);

    return Status;
}

/**
*
*/

int32 OpenSrcFile(void)
{
    int RtnCode;
    
    // Check to see if input file can be found and opened
    SrcFileDesc = fopen(SrcFilename, "r");

    if (SrcFileDesc == NULL)
    {
        printf("'%s' was not opened\n", SrcFilename);
        return FAILED;
    }
    
    /* Obtain time of object file's last modification */
    RtnCode = stat(SrcFilename, &SrcFileStats);
    
    SrcFileTimeInScEpoch = SrcFileStats.st_mtime + EpochDelta;
    
    if (Verbose) printf("Original Source File Modification Time: %s\n", ctime(&SrcFileStats.st_mtime));
    if (Verbose) printf("Source File Modification Time in Seconds since S/C Epoch: %ld (0x%08X)\n", SrcFileTimeInScEpoch, SrcFileTimeInScEpoch);

    return SUCCESS;
}

int32 OpenDstFile(void)
{
    // Check to see if output file can be opened and written
    DstFileDesc = fopen(DstFilename, "w");

    if (DstFileDesc == NULL)
    {
        printf("'%s' was not opened\n", DstFilename);
        return FAILED;
    }
    
    return SUCCESS;
}

/**
*
*/

int32 GetElfHeader(void)
{
    int32   Status = SUCCESS;
    size_t  NumHdrsRead = 0;
    char    VerboseStr[60];
    int32   EndiannessCheck=0x01020304;
    
    if (((char *)&EndiannessCheck)[0] == 0x01)
    {
        ThisMachineIsLittleEndian = FALSE;
    }
    else if (((char *)&EndiannessCheck)[0] != 0x04)
    {
        printf("Unable to determine endianness of this machine! (0x%02x, 0x%02x, 0x%02x, 0x%02x)\n",
               ((char *)&EndiannessCheck)[0], ((char *)&EndiannessCheck)[1], ((char *)&EndiannessCheck)[2], ((char *)&EndiannessCheck)[3]);
        return FAILED;
    }       
    
    NumHdrsRead = fread(&ElfHeader, sizeof(ElfHeader), 1, SrcFileDesc);
    
    if (NumHdrsRead != 1)
    {
        printf("Experienced error attempting to read ELF Header from file '%s'\n", SrcFilename);
        return FAILED;
    }
   
    if (Verbose) printf("ELF Header:\n");
    if (Verbose) printf("   e_ident[EI_MAG0..3] = 0x%02x,%c%c%c\n", 
                        ElfHeader.e_ident[EI_MAG0], ElfHeader.e_ident[EI_MAG1], 
                        ElfHeader.e_ident[EI_MAG2], ElfHeader.e_ident[EI_MAG3]);
    
    /* Verify the ELF file magic number */
    if (ElfHeader.e_ident[EI_MAG0] != ELFMAG0) Status = FAILED;         
    if (ElfHeader.e_ident[EI_MAG1] != ELFMAG1) Status = FAILED;         
    if (ElfHeader.e_ident[EI_MAG2] != ELFMAG2) Status = FAILED;         
    if (ElfHeader.e_ident[EI_MAG3] != ELFMAG3) Status = FAILED;
    
    if (Status == FAILED)
    {
        printf("Source File '%s' does not have an ELF Magic Number\n", SrcFilename);
        printf("If this object file was compiled on a PC under cygwin, then it is probably in COFF format.\n");
        printf("To convert it to an elf file, use the following command:\n");
        printf("./objcopy -O elf32-little %s %s.elf\n", SrcFilename, SrcFilename);
        printf("then try running this utility again on the %s.elf file\n", SrcFilename);
        return Status;
    }
             
    /* Verify the processor class type */                    
    switch (ElfHeader.e_ident[EI_CLASS])
    {
        case ELFCLASSNONE:
            sprintf(VerboseStr, "ELFCLASSNONE (0)");
            Status = FAILED;
            break;
            
        case ELFCLASS32:
            sprintf(VerboseStr, "ELFCLASS32 (1)");
            break;
            
        case ELFCLASS64:
            sprintf(VerboseStr, "ELFCLASS64 (2)");
            Status = FAILED;
            break;
            
        default:
            sprintf(VerboseStr, "Invalid Class (%d)", ElfHeader.e_ident[EI_CLASS]);
            Status = FAILED;
            break;
    }
    
    if (Status == FAILED)
    {
        printf("Source file '%s' contains objects of class type '%s' which is unsupported by this utility\n", 
               SrcFilename, VerboseStr);
        return Status;       
    }
           
    if (Verbose) printf("   e_ident[EI_CLASS] = %s\n", VerboseStr);       
    
    /* Verify Data Encoding type */
    switch (ElfHeader.e_ident[EI_DATA])
    {
        case ELFDATANONE:
            sprintf(VerboseStr, "ELFDATANONE");
            Status = FAILED;
            break;
            
        case ELFDATA2LSB:
            sprintf(VerboseStr, "ELFDATA2LSB (Little-Endian)");
            TargetMachineIsLittleEndian = TRUE;
            if (ThisMachineIsLittleEndian == FALSE)
            {
                ByteSwapRequired = TRUE;
            }
            break;
            
        case ELFDATA2MSB:
            sprintf(VerboseStr, "ELFDATA2MSB (Big-Endian)");
            TargetMachineIsLittleEndian = FALSE;
            if (ThisMachineIsLittleEndian == TRUE)
            {
                ByteSwapRequired = TRUE;
            }
            break;
            
        default:
            sprintf(VerboseStr, "Unknown Data Encoding Type (%d)", ElfHeader.e_ident[EI_DATA]);
            Status = FAILED;
            break;
    }
    
    if (Status == FAILED)
    {
        printf("Source file '%s' contains data encoded with '%s'\n", SrcFilename, VerboseStr);
        return Status;
    }
    
    if (Verbose) printf("   e_ident[EI_DATA] = %s\n", VerboseStr);
    
    /* Verify ELF Header Version */
    if (ElfHeader.e_ident[EI_VERSION] != EV_CURRENT)
    {
        printf("Source file '%s' is improper ELF header version (%d)\n", SrcFilename, ElfHeader.e_ident[EI_VERSION]);
        return FAILED;
    }
    
    if (Verbose) printf("   e_ident[EI_VERSION] = %d\n", ElfHeader.e_ident[EI_VERSION]);
    
    if (ByteSwapRequired == TRUE)
    {
        SwapElfHeader();
    }
    
    /* Verify ELF Type */
    Status = GetStringFromMap(&VerboseStr[0], e_type_Map, (int32)ElfHeader.e_type);

    if (Status == FAILED)
    {
        printf("Error in source file '%s' - %s\n", SrcFilename, VerboseStr);
        return FAILED;
    }
    
    if (Verbose) printf("   e_type = %s\n", VerboseStr);
    
    /* Verify machine type */
    Status = GetStringFromMap(&VerboseStr[0], e_machine_Map, (int32)ElfHeader.e_machine);
    
    if (Status == FAILED)
    {
        printf("Error in source file '%s' - %s\n", SrcFilename, VerboseStr);
        return FAILED;
    }
    
    if (Verbose) printf("   e_machine = %s\n", VerboseStr);
    
    /* Verify ELF Object File Version */
    if (ElfHeader.e_version != EV_CURRENT)
    {
        printf("Error in source file '%s' - Improper ELF object version (%d)\n", SrcFilename, ElfHeader.e_version);
        return FAILED;
    }
    
    if (Verbose) printf("   e_version = %d\n", ElfHeader.e_version);
    
    /* Output, if requested, other header entries without verification */
    if (Verbose) printf("   e_entry = 0x%08x\n", (uint32)ElfHeader.e_entry);
    if (Verbose) printf("   e_phoff = 0x%08x (%d)\n", ElfHeader.e_phoff, ElfHeader.e_phoff);
    if (Verbose) printf("   e_shoff = 0x%08x (%d)\n", ElfHeader.e_shoff, ElfHeader.e_shoff);
    if (Verbose) printf("   e_flags = 0x%08x\n", ElfHeader.e_flags);
    if (Verbose) printf("   e_ehsize = %d\n", ElfHeader.e_ehsize);
    if (Verbose) printf("   e_phentsize = %d\n", ElfHeader.e_phentsize);
    if (Verbose) printf("   e_phnum = %d\n", ElfHeader.e_phnum);
    if (Verbose) printf("   e_shentsize = %d\n", ElfHeader.e_shentsize);
    if (Verbose) printf("   e_shnum = %d\n", ElfHeader.e_shnum);
    if (Verbose) printf("   e_shstrndx = %d\n", ElfHeader.e_shstrndx);
    
    return Status;
}

/**
*
*/

int32 GetSectionHeader(int32 SectionIndex, Elf32_Shdr *SectionHeader)
{
    int32 Status = SUCCESS;
    size_t NumHdrsRead = 0;
    char VerboseStr[60];
    int32 SeekOffset = ElfHeader.e_shoff;
    int32 i=0;

    if (SectionIndex > 0)
    {
        SeekOffset = SeekOffset + (SectionIndex * ElfHeader.e_shentsize);
    }
    
    Status = fseek(SrcFileDesc, SeekOffset, SEEK_SET);
    
    if (Status != 0)
    {
        printf("Error locating Section Header #%d in file '%s'\n", SectionIndex, SrcFilename);
        return FAILED;
    }
    
    NumHdrsRead = fread(SectionHeader, sizeof(Elf32_Shdr), 1, SrcFileDesc);
    
    if (NumHdrsRead != 1)
    {
        printf("Experienced error attempting to read Section Header #%d from file '%s'\n", SectionIndex, SrcFilename);
        return FAILED;
    }
    
    if (ByteSwapRequired == TRUE) SwapSectionHeader(SectionHeader);
   
    if ((SectionHeaderStringTableDataOffset != 0) && (SectionHeader->sh_name != 0))
    {
        if (Verbose) printf("Section Header #%d:\n", SectionIndex);
        
        SeekOffset = SectionHeaderStringTableDataOffset + SectionHeader->sh_name;
        if (Verbose) printf("   sh_name       = 0x%08x - ", SectionHeader->sh_name);
        fseek(SrcFileDesc, SeekOffset, SEEK_SET);
        
        while ((VerboseStr[i] = fgetc(SrcFileDesc)) != '\0')
        {
            i++;
        }
        if (Verbose) printf("%s\n", VerboseStr);
        
        /* Save the name for later reference */
        strncpy(SectionNamePtrs[SectionIndex], VerboseStr, (MAX_SECTION_HDR_NAME_LEN-1));
        SectionNamePtrs[SectionIndex][(MAX_SECTION_HDR_NAME_LEN-1)] = '\0';
        
        switch (SectionHeader->sh_type)
        {
            case SHT_NULL:
                sprintf(VerboseStr, "SHT_NULL (0)");
                break;
                
            case SHT_PROGBITS:
                sprintf(VerboseStr, "SHT_PROGBITS (1)");
                break;
                
            case SHT_SYMTAB:
                SymbolTableDataOffset = SectionHeader->sh_offset + sizeof(Elf32_Sym);
                SymbolTableEntrySize = SectionHeader->sh_entsize;
                NumSymbols = (SectionHeader->sh_size / SectionHeader->sh_entsize) - 1;
                sprintf(VerboseStr, "SHT_SYMTAB (2) - # Symbols = %d", NumSymbols);
                break;
                
            case SHT_STRTAB:
                sprintf(VerboseStr, "SHT_STRTAB (3)");
                if (SectionIndex != ElfHeader.e_shstrndx)
                {
                    StringTableDataOffset = SectionHeader->sh_offset;
                }
                break;
                
            case SHT_RELA:
                sprintf(VerboseStr, "SHT_RELA (4)");
                break;
                
            case SHT_HASH:
                sprintf(VerboseStr, "SHT_HASH (5)");
                break;
                
            case SHT_DYNAMIC:
                sprintf(VerboseStr, "SHT_DYNAMIC (6)");
                break;
                
            case SHT_NOTE:
                sprintf(VerboseStr, "SHT_NOTE (7)");
                break;
                
            case SHT_NOBITS:
                sprintf(VerboseStr, "SHT_NOBITS (8)");
                break;
                
            case SHT_REL:
                sprintf(VerboseStr, "SHT_REL (9)");
                break;
                
            case SHT_SHLIB:
                sprintf(VerboseStr, "SHT_SHLIB (10)");
                break;
                
            case SHT_DYNSYM:
                sprintf(VerboseStr, "SHT_DYNSYM (11)");
                break;
                
            case SHT_INIT_ARRAY:
                sprintf(VerboseStr, "SHT_INIT_ARRAY (14)");
                break;
                
            case SHT_FINI_ARRAY:
                sprintf(VerboseStr, "SHT_FINI_ARRAY (15)");
                break;
                
            case SHT_PREINIT_ARRAY:
                sprintf(VerboseStr, "SHT_PREINIT_ARRAY (16)");
                break;
                
            case SHT_GROUP:
                sprintf(VerboseStr, "SHT_GROUP (17)");
                break;
                
            case SHT_SYMTAB_SHNDX:
                sprintf(VerboseStr, "SHT_SYMTAB_SHNDX (18)");
                break;
                
            default:
                sprintf(VerboseStr, "Unknown (%d)", SectionHeader->sh_type);
                break;
        }
        
        if (Verbose) printf("   sh_type       = %s\n", VerboseStr);
        
        sprintf(VerboseStr, "/");
        if ((SectionHeader->sh_flags & SHF_WRITE) == SHF_WRITE)
        {
            sprintf(VerboseStr, "SHF_WRITE/");
        }
        
        if ((SectionHeader->sh_flags & SHF_ALLOC) == SHF_ALLOC)
        {
            strcat(VerboseStr, "SHF_ALLOC/");
        }
        
        if ((SectionHeader->sh_flags & SHF_EXECINSTR) == SHF_EXECINSTR)
        {
            strcat(VerboseStr, "SHF_EXECINSTR/");
        }
        if (Verbose) printf("   sh_flags      = %s\n", VerboseStr);
        
        
        if (Verbose) printf("   sh_addr       = 0x%08x\n", (uint32)SectionHeader->sh_addr);
        if (Verbose) printf("   sh_offset     = 0x%08x\n", SectionHeader->sh_offset);
        if (Verbose) printf("   sh_size       = 0x%08x\n", SectionHeader->sh_size);
        if (Verbose) printf("   sh_link       = 0x%08x\n", SectionHeader->sh_link);
        if (Verbose) printf("   sh_info       = 0x%08x\n", SectionHeader->sh_info);
        if (Verbose) printf("   sh_addralign  = 0x%08x\n", SectionHeader->sh_addralign);
        if (Verbose) printf("   sh_entsize    = 0x%08x\n", SectionHeader->sh_entsize);
    }
    
    return Status;
}

/**
*
*/

int32 GetSymbol(int32 SymbolIndex, Elf32_Sym *Symbol)
{
    int32 Status = SUCCESS;
    int32 NumSymRead = 0;
    int32 SeekOffset = SymbolTableDataOffset + (SymbolIndex * SymbolTableEntrySize);
    char VerboseStr[60];
    int32 i=0;
    
    Status = fseek(SrcFileDesc, SeekOffset, SEEK_SET);
    
    if (Status != 0)
    {
        printf("Error locating Symbol #%d in file '%s'\n", SymbolIndex, SrcFilename);
        return FAILED;
    }
    
    NumSymRead = fread(Symbol, sizeof(Elf32_Sym), 1, SrcFileDesc);
    
    if (NumSymRead != 1)
    {
        printf("Experienced error attempting to read Symbol #%d from file '%s'\n", SymbolIndex, SrcFilename);
        return FAILED;
    }
    
    if (ByteSwapRequired) SwapSymbol(Symbol);
   
    if (Verbose) printf("Symbol #%d:\n", (SymbolIndex+1));
    
    SeekOffset = StringTableDataOffset + Symbol->st_name;
    if (Verbose) printf("   st_name  = 0x%08x - ", Symbol->st_name);
    fseek(SrcFileDesc, SeekOffset, SEEK_SET);
    
    while ((i<sizeof(VerboseStr)) && ((VerboseStr[i] = fgetc(SrcFileDesc)) != '\0'))
    {
        i++;
    }
    
    VerboseStr[i] = '\0'; /* Just in case i=sizeof(VerboseStr) */
    
    SymbolNames[SymbolIndex] = malloc(i+1);
    strcpy(SymbolNames[SymbolIndex], VerboseStr);
    
    if ((strcmp(VerboseStr, TBL_DEF_SYMBOL_NAME) == 0) ||
        (strcmp(&VerboseStr[1], TBL_DEF_SYMBOL_NAME) == 0))
    {
        if (Verbose) printf("*** %s ***\n", SymbolNames[SymbolIndex]);
        TblDefSymbolIndex = SymbolIndex;
    }
    else
    {
        if (Verbose) printf("%s\n", SymbolNames[SymbolIndex]);
    }
        
    if (Verbose) printf("   st_value = 0x%08x\n", (uint32)Symbol->st_value);
    if (Verbose) printf("   st_size  = 0x%08x\n", Symbol->st_size);
    if (Verbose) printf("   st_info  = 0x%02x\n", Symbol->st_info);
    if (Verbose) printf("   st_other = 0x%02x\n", Symbol->st_other);
    if (Verbose) printf("   st_shndx = 0x%04x\n", Symbol->st_shndx);
    
    return Status;
}


/**
*
*/

void SwapElfHeader(void)
{
    SwapUInt16(&ElfHeader.e_type);
    SwapUInt16(&ElfHeader.e_machine);
    SwapUInt32((uint32 *)&ElfHeader.e_version);
    SwapUInt32((uint32 *)&ElfHeader.e_entry);
    SwapUInt32((uint32 *)&ElfHeader.e_phoff);
    SwapUInt32((uint32 *)&ElfHeader.e_shoff);
    SwapUInt32((uint32 *)&ElfHeader.e_flags);
    SwapUInt16(&ElfHeader.e_ehsize);
    SwapUInt16(&ElfHeader.e_phentsize);
    SwapUInt16(&ElfHeader.e_phnum);
    SwapUInt16(&ElfHeader.e_shentsize);
    SwapUInt16(&ElfHeader.e_shnum);
    SwapUInt16(&ElfHeader.e_shstrndx);
}

/**
*
*/

void  SwapSectionHeader(Elf32_Shdr *SectionHeader)
{
    SwapUInt32((uint32 *)&SectionHeader->sh_name);
    SwapUInt32((uint32 *)&SectionHeader->sh_type);
    SwapUInt32((uint32 *)&SectionHeader->sh_flags);
    SwapUInt32((uint32 *)&SectionHeader->sh_addr);
    SwapUInt32((uint32 *)&SectionHeader->sh_offset);
    SwapUInt32((uint32 *)&SectionHeader->sh_size);
    SwapUInt32((uint32 *)&SectionHeader->sh_link);
    SwapUInt32((uint32 *)&SectionHeader->sh_info);
    SwapUInt32((uint32 *)&SectionHeader->sh_addralign);
    SwapUInt32((uint32 *)&SectionHeader->sh_entsize);
}

/**
*
*/

void  SwapSymbol(Elf32_Sym *Symbol)
{
    SwapUInt32((uint32 *)&Symbol->st_name);
    SwapUInt32((uint32 *)&Symbol->st_value);
    SwapUInt32((uint32 *)&Symbol->st_size);
    SwapUInt16((uint16 *)&Symbol->st_shndx);
}

/**
*
*/

void SwapUInt16(uint16 *ValueToSwap)
{
    uint8 *BytePtr = (uint8 *)ValueToSwap;
    uint8  TempByte = BytePtr[1];
    BytePtr[1] = BytePtr[0];
    BytePtr[0] = TempByte;
}

/**
*
*/

void SwapUInt32(uint32 *ValueToSwap)
{
    uint8 *BytePtr = (uint8 *)ValueToSwap;
    uint8  TempByte = BytePtr[3];
    BytePtr[3] = BytePtr[0];
    BytePtr[0] = TempByte;
    TempByte   = BytePtr[2];
    BytePtr[2] = BytePtr[1];
    BytePtr[1] = TempByte;
}

/**
*
*/

int32 GetStringFromMap(char *Result, ElfStrMap *Map, int32 Key)
{
    int32 Status = FAILED;
    
    while ((Map->String[0] != '*') && (Status == FAILED))
    {
        if (Map->Value == Key)
        {
            Status = SUCCESS;
            strcpy(Result, Map->String);
        }
        else
        {
            Map++;
        }
    }
    
    if (Status == FAILED)
    {
        sprintf(Result, Map->String, Key); 
    }
    
    return Status;
}

/**
*
*/

int32 GetTblDefInfo(void)
{
    int32 Status = SUCCESS;
    uint32 SeekOffset = 0;
    int32 NumDefsRead = 0;
    
    /* Read the data to be used to format the CFE File and Table Headers */
    if ((SymbolPtrs[TblDefSymbolIndex]->st_size != sizeof(CFE_TBL_FileDef_t)) && (SymbolPtrs[TblDefSymbolIndex]->st_size != 0))
    {
        printf("Error! '%s' is not properly defined in '%s'.  Size of object is incorrect (%d).\n", 
               TBL_DEF_SYMBOL_NAME, SrcFilename, SymbolPtrs[TblDefSymbolIndex]->st_size);
        Status = FAILED;
    }
    else
    {
        SeekOffset = SectionHeaderPtrs[SymbolPtrs[TblDefSymbolIndex]->st_shndx]->sh_offset + 
                     (uint32)SymbolPtrs[TblDefSymbolIndex]->st_value;
        fseek(SrcFileDesc, SeekOffset, SEEK_SET);
        NumDefsRead = fread(&TblFileDef, sizeof(CFE_TBL_FileDef_t), 1, SrcFileDesc);
        
        if (NumDefsRead != 1)
        {
            printf("Error! Unable to read data content of '%s' from '%s'.\n", TBL_DEF_SYMBOL_NAME, SrcFilename);
            Status = FAILED;
        }

        if (ByteSwapRequired) SwapUInt32(&TblFileDef.ObjectSize);
        
        if (Verbose) 
        {
            printf("Table Defined as follows:\n");
            printf("   Data Object: %s\n", TblFileDef.ObjectName);
            printf("   Table Name : '%s'", TblFileDef.TableName);
            if (TableNameOverride == TRUE)
            {
                printf(" overridden with : '%s'", TableName);
            }
            printf("\n");
            printf("   Description: '%s'", TblFileDef.Description);
            if (DescriptionOverride == TRUE)
            {
                printf(" overridden with : '%s'", Description);
            }
            printf("\n");
            printf("   Output File: %s\n", TblFileDef.TgtFilename);
            printf("   Object Size: %d (0x%08x)\n", TblFileDef.ObjectSize, TblFileDef.ObjectSize);
        }
    }
    
    return Status;
}

/**
*
*/

int32 LocateAndReadUserObject(void)
{
    int32  Status = SUCCESS;
    int32  i=0;
    int32  j=0;
    uint32 SeekOffset = 0;
    uint8  AByte;
    
    /* Search the symbol table for the user defined object */
    if (Verbose) printf("\nTrying to match ObjectName '%s'... (length %d)",TblFileDef.ObjectName,strlen(TblFileDef.ObjectName));
    while (i<NumSymbols)
    {
        if (Verbose) printf("\nSymbol Search loop %d: SymbolName ='%s' ",i,SymbolNames[i]);
        /* Check to see if the symbol names match as far as the ObjectName is defined */
        if (strncmp(SymbolNames[i], TblFileDef.ObjectName, strlen(TblFileDef.ObjectName)) == 0)
        {
            if (Verbose) printf("Found ObjectName '%s' inside SymbolName '%s'\n",TblFileDef.ObjectName, SymbolNames[i]);
            /* Check to see if the Symbol Name happens to have one extra character */
            if ((strlen(SymbolNames[i]) - strlen(TblFileDef.ObjectName)) == 1)
            {
                if (Verbose) printf("Found an extra character...\n");
                /* If the character is non-printable, then we have a match */
                if (isprint(SymbolNames[i][strlen(SymbolNames[i])]) == 0)
                {
                    if (Verbose) printf("...and it's unprintable!");
                    break;
                }
            }
            /* Check to see if the Symbol Name is an exact match */
            else if ((strlen(SymbolNames[i]) - strlen(TblFileDef.ObjectName)) == 0)
            {
                if (Verbose) printf("\nFound an exact match! Symbol='%s' Object='%s'\n",SymbolNames[i],TblFileDef.ObjectName);
                break;
            }
        }

        if (Verbose)
        {
            printf("strstr[%d] = %d; strlenSN = %d; strlenON = %d\n",i,strstr(SymbolNames[i], TblFileDef.ObjectName),
                                                                     strlen(SymbolNames[i]), 
                                                                     strlen(TblFileDef.ObjectName));
        }
        
        i++;
    }
    
    if (Verbose)
    {
        printf("\ni = %d, NumSymbols = %d\n", i, NumSymbols);
        if (i < NumSymbols)
        {
          printf("\nSymbolName = '%s', ObjectName = '%s'\n",SymbolNames[i], TblFileDef.ObjectName);
          printf("\nSymbolName length = %d, ObjectName length = %d\n",strlen(SymbolNames[i]), strlen(TblFileDef.ObjectName));
        }
    }
    
    if (i == NumSymbols)
    {
        printf("Error! Unable to find '%s' object in '%s'\n", TblFileDef.ObjectName, SrcFilename);
        Status = FAILED;
    }
    else
    {
        if (Verbose) printf("Found '%s' object as Symbol #%d\n", TblFileDef.ObjectName, (i+1));
        UserObjSymbolIndex = i;
        
        if (strcmp(SectionNamePtrs[SymbolPtrs[UserObjSymbolIndex]->st_shndx], ".bss") == 0)
        {
            if (Verbose) printf("Table contents are in '.bss' section and are assumed to be all zeros.\n");
            TableDataIsAllZeros = TRUE;
        
            if (Verbose)
            {
                printf("Object Data:\n");
                for (i=0; i<SymbolPtrs[UserObjSymbolIndex]->st_size; i++)
                {
                    printf(" 0x%02x", 0);
                    j++;
                    if (j==16)
                    {
                        printf("\n");
                        j=0;
                    }
                }
            }
        }
        else
        {
            /* Locate data associated with symbol */
            SeekOffset = SectionHeaderPtrs[SymbolPtrs[UserObjSymbolIndex]->st_shndx]->sh_offset + 
                         (uint32)SymbolPtrs[UserObjSymbolIndex]->st_value;
            fseek(SrcFileDesc, SeekOffset, SEEK_SET);
        
            /* Determine if the elf file contained the size of the object */
            if (SymbolPtrs[UserObjSymbolIndex]->st_size != 0)
            {
                /* Check to see if the size in the elf file agrees with the size specified in our table def structure */
                if (SymbolPtrs[UserObjSymbolIndex]->st_size != TblFileDef.ObjectSize)
                {
                    printf("ELF file indicates object '%s' is of size %d but table definition structure indicates size %d",
                           TblFileDef.ObjectName, SymbolPtrs[UserObjSymbolIndex]->st_size, TblFileDef.ObjectSize);
                    if (TblFileDef.ObjectSize < SymbolPtrs[UserObjSymbolIndex]->st_size)
                    {
                        SymbolPtrs[UserObjSymbolIndex]->st_size = TblFileDef.ObjectSize;
                    }
                           
                    printf("Size of %d is assumed.\n", SymbolPtrs[UserObjSymbolIndex]->st_size);
                }
            }
            else
            {
                /* Since the size is not available from the elf file, assume the value in the table def structure is right */
                SymbolPtrs[UserObjSymbolIndex]->st_size = TblFileDef.ObjectSize;
            }
        
            if (Verbose)
            {
                printf("Object Data:\n");
                for (i=0; i<SymbolPtrs[UserObjSymbolIndex]->st_size; i++)
                {
                    AByte = fgetc(SrcFileDesc);
                    printf(" 0x%02x", AByte);
                    j++;
                    if (j==16)
                    {
                        printf("\n");
                        j=0;
                    }
                }
                /* Reset the file pointer */
                fseek(SrcFileDesc, SeekOffset, SEEK_SET);
            }
        }
    }
    
    return Status;
}

/**
*
*/

int32 OutputDataToTargetFile()
{
    int32 Status = SUCCESS;
    uint8 AByte = 0;
    int32 i = 0;

    /* Create the standard header */
    FileHeader.ContentType = 0x63464531;
    FileHeader.SubType = CFE_FS_TBL_IMG_SUBTYPE;
    FileHeader.Length = sizeof(CFE_FS_Header_t);
    
    if (ScIDSpecified == TRUE)
    {
        FileHeader.SpacecraftID = SpacecraftID;
    }
    else
    {
        FileHeader.SpacecraftID = 0;
    }
    
    if (ProcIDSpecified == TRUE)
    {
        FileHeader.ProcessorID = ProcessorID;
    }
    else
    {
        FileHeader.ProcessorID = 0;
    }
    
    if (AppIDSpecified == TRUE)
    {
        FileHeader.ApplicationID = ApplicationID;
    }
    else
    {
        FileHeader.ApplicationID = 0;
    }
    
    if (EnableTimeTagInHeader)
    {
        FileHeader.TimeSeconds = SrcFileTimeInScEpoch;
        FileHeader.TimeSubSeconds = 0;
    }
    else
    {
        FileHeader.TimeSeconds = 0;
        FileHeader.TimeSubSeconds = 0;
    }
    
    memset(FileHeader.Description, 0, CFE_FS_HDR_DESC_MAX_LEN);
    
    if (DescriptionOverride == TRUE)
    {
        strcpy(FileHeader.Description, Description);
    }
    else
    {
        strcpy(FileHeader.Description, TblFileDef.Description);
    }
    
    /* If this machine is little endian, the CFE header must be swapped */
    if (ThisMachineIsLittleEndian == TRUE)
    {
        if (Verbose) printf("\ncFE Headers are being byte-swapped because this machine is 'Little Endian'\n");
        SwapUInt32(&FileHeader.ContentType);
        SwapUInt32(&FileHeader.SubType);
        SwapUInt32(&FileHeader.Length);
        SwapUInt32(&FileHeader.SpacecraftID);
        SwapUInt32(&FileHeader.ProcessorID);
        SwapUInt32(&FileHeader.ApplicationID);
        SwapUInt32(&FileHeader.TimeSeconds);
        SwapUInt32(&FileHeader.TimeSubSeconds);
    }
    
    /* Create the standard cFE Table Header */
    memset(&TableHeader, 0, sizeof(TableHeader));
    TableHeader.NumBytes = SymbolPtrs[UserObjSymbolIndex]->st_size;
    
    if (TableNameOverride == TRUE)
    {
        strcpy(TableHeader.TableName, TableName);
    }
    else
    {
        strcpy(TableHeader.TableName, TblFileDef.TableName);
    }
    
    /* If this machine is little endian, the TBL header must be swapped */
    if (ThisMachineIsLittleEndian == TRUE)
    {
        SwapUInt32(&TableHeader.Reserved);
        SwapUInt32(&TableHeader.Offset);
        SwapUInt32(&TableHeader.NumBytes);
    }
    
    /* Output the two headers to the target file */
    fwrite(&FileHeader.ContentType, sizeof(uint32), 1, DstFileDesc);
    fwrite(&FileHeader.SubType, sizeof(uint32), 1, DstFileDesc);
    fwrite(&FileHeader.Length, sizeof(uint32), 1, DstFileDesc);
    fwrite(&FileHeader.SpacecraftID, sizeof(uint32), 1, DstFileDesc);
    fwrite(&FileHeader.ProcessorID, sizeof(uint32), 1, DstFileDesc);
    fwrite(&FileHeader.ApplicationID, sizeof(uint32), 1, DstFileDesc);
    fwrite(&FileHeader.TimeSeconds, sizeof(uint32), 1, DstFileDesc);
    fwrite(&FileHeader.TimeSubSeconds, sizeof(uint32), 1, DstFileDesc);
    fwrite(&FileHeader.Description[0], CFE_FS_HDR_DESC_MAX_LEN, 1, DstFileDesc);
    
    if ((ByteAlignFileHeaders) && (CFE_FS_HDR_DESC_MAX_LEN%4 != 0))
    {
        AByte = 0;
        for (i=0; i<(4-(CFE_FS_HDR_DESC_MAX_LEN%4)); i++)
        {
            fwrite(&AByte, 1, 1, DstFileDesc);
        }
    }

    fwrite(&TableHeader.Reserved, sizeof(uint32), 1, DstFileDesc);
    fwrite(&TableHeader.Offset, sizeof(uint32), 1, DstFileDesc);
    fwrite(&TableHeader.NumBytes, sizeof(uint32), 1, DstFileDesc);
    fwrite(&TableHeader.TableName[0], CFE_TBL_MAX_FULL_NAME_LEN, 1, DstFileDesc);
    
    if ((ByteAlignFileHeaders) && (CFE_TBL_MAX_FULL_NAME_LEN%4 != 0))
    {
        AByte = 0;
        for (i=0; i<(4-(CFE_TBL_MAX_FULL_NAME_LEN%4)); i++)
        {
            fwrite(&AByte, 1, 1, DstFileDesc);
        }
    }

    /* Output the data from the object file */
    if (TableDataIsAllZeros)
    {
        AByte = 0;
        for (i=0; i<SymbolPtrs[UserObjSymbolIndex]->st_size; i++)
        {
            fwrite(&AByte, 1, 1, DstFileDesc);
        }
    }
    else
    {
        for (i=0; i<SymbolPtrs[UserObjSymbolIndex]->st_size; i++)
        {
            AByte = fgetc(SrcFileDesc);
            fwrite(&AByte, 1, 1, DstFileDesc);
        }
    }
    
    return Status;
}
