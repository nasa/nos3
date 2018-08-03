#include <string.h>
#include "sbn_app.h"
#include "sbn_main_events.h"
#include "sbn_loader.h"

#ifdef _osapi_confloader_
static int SBN_ConfLoaderRowCallback(const char *filename, int linenum,
    const char *header, const char *row[], int fieldcount, void *opaque)
{
    int32   status = 0;
    uint32  ModuleId = 0;
    cpuaddr StructAddr = 0;
    int ProtocolId = 0;

    DEBUG_START();

    if (fieldcount != 4)
    {
        OS_printf("error: invalid field count %d (expected 4)\n", fieldcount); /* TODO: event */
        return OS_SUCCESS;
    }

    ProtocolId = atoi(row[0]);

    if (ProtocolId < 0 || ProtocolId > SBN_MAX_INTERFACE_TYPES)
    {   
        OS_printf("%s:Invalid protocol id %d\n",
            CFE_CPU_NAME, ProtocolId);
        return OS_SUCCESS;
    }/* end if */

    status = OS_ModuleLoad(&ModuleId, row[1], row[2]);
    if(status != OS_SUCCESS)
    {
        return OS_SUCCESS;
    }/* end if */

    status = OS_SymbolLookup(&StructAddr, row[3]);
    if(status != OS_SUCCESS)
    {
        OS_printf("Failed to find symbol %s\n", row[3]);
        return OS_SUCCESS;
    }/* end if */

    SBN.IfOps[ProtocolId] = (SBN_InterfaceOperations *)StructAddr;

    return OS_SUCCESS;
}

static int SBN_ConfLoaderErrorCallback(const char *filename, int linenum,
    const char *errmessage, void *opaque)
{
    OS_printf("error: %s\n", errmessage); /* TODO: event */
    return OS_SUCCESS;
}

int32 SBN_ReadModuleFile(void)
{
    int32 status = 0, id = 0;

    status = OS_ConfLoaderAPIInit();
    if (status != OS_SUCCESS)
    {
        return status;
    }

    status = OS_ConfLoaderInit(&id, "sbn_module");
    if (status != OS_SUCCESS)
    {
        return status;
    }

    status = OS_ConfLoaderSetRowCallback(id, SBN_ConfLoaderRowCallback, NULL);
    if (status != OS_SUCCESS)
    {
        return status;
    }

    status = OS_ConfLoaderSetErrorCallback(id, SBN_ConfLoaderErrorCallback,
        NULL);
    if (status != OS_SUCCESS)
    {
        return status;
    }

    return OS_ConfLoaderLoad(id, SBN_NONVOL_MODULE_FILENAME);
}

#else /* ! _osapi_confloader_ */

/**
 * Reads a file describing the interface modules that
 * must be loaded.
 *
 * The format is the same as that for the peer file.
 */
int32 SBN_ReadModuleFile(void)
{
    /* A buffer for a line in a file */
    static char     SBN_ModuleData[SBN_MODULE_FILE_LINE_SIZE];
    int             BuffLen = 0, /* Length of the current buffer */
                    ModuleFile = 0;
    char            c = '\0';
    uint32          LineNum = 0;

    DEBUG_START();

    ModuleFile = OS_open(SBN_NONVOL_MODULE_FILENAME, OS_READ_ONLY, 0);

    if(ModuleFile == OS_ERROR)
    {
        return SBN_ERROR;
    }/* end if */

    CFE_PSP_MemSet(SBN_ModuleData, 0x0, SBN_MODULE_FILE_LINE_SIZE);
    BuffLen = 0;

    /*
     ** Parse the lines from the file
     */
    while(1)
    {
        OS_read(ModuleFile, &c, 1);

        if (c == '!')
        {
            break;
        }/* end if */

        switch(c)
        {
            case '\n':
            case ' ':
            case '\t':
                /* ignore whitespace */
                break;

            case ',':
                /*
                 ** replace the field delimiter with a space
                 ** This is used for the sscanf string parsing
                 */
                SBN_ModuleData[BuffLen] = ' ';
                if (BuffLen < (SBN_MODULE_FILE_LINE_SIZE - 1))
                {
                    BuffLen++;
                }/* end if */
                break;
            case ';':
                /* Send the line to the file parser */
                if (SBN_ParseModuleEntry(SBN_ModuleData, LineNum) == -1)
                    return SBN_ERROR;
                LineNum++;
                CFE_PSP_MemSet(SBN_ModuleData, 0x0, SBN_MODULE_FILE_LINE_SIZE);
                BuffLen = 0;
                break;
            default:
                /* Regular data gets copied in */
                SBN_ModuleData[BuffLen] = c;
                if (BuffLen < (SBN_MODULE_FILE_LINE_SIZE - 1))
                {
                    BuffLen++;
                }/* end if */
        }/* end switch */
    }/* end while */

    OS_close(ModuleFile);

    return SBN_OK;
}/* end SBN_ReadModuleFile */

/**
 * Parses a module file entry to obtain the protocol id, name,
 * path, and function structure for an interface type.
 *
 * @param FileEntry  Interface description line as read from file
 * @param LineNum    The line number in the module file

 * @return  SBN_OK on success, SBN_ERROR on error
 */
int32 SBN_ParseModuleEntry(char *FileEntry, uint32 LineNum)
{
    int  ProtocolId = 0;
    char    ModuleName[50];
    char    ModuleFile[50];
    char    StructName[50];
    int     ScanfStatus = 0;

    int32   ReturnCode = 0;
    uint32  ModuleId = 0;
    cpuaddr StructAddr = 0;

    DEBUG_START();

    /* switch on protocol ID */
    ScanfStatus = sscanf(FileEntry, "%d %s %s %s",
        &ProtocolId, ModuleName, ModuleFile, StructName);

    /*
    ** Check to see if the correct number of items were parsed
    */
    if (ScanfStatus != 4)
    {
        OS_printf("%s:Invalid SBN module file line,exp %d items,found %d",
            CFE_CPU_NAME, 4, ScanfStatus);
        return SBN_ERROR;
    }/* end if */

    if (ProtocolId < 0 || ProtocolId > SBN_MAX_INTERFACE_TYPES)
    {
        OS_printf("%s:Invalid protocol id %d",
            CFE_CPU_NAME, ProtocolId);
        return SBN_ERROR;
    }/* end if */

    ReturnCode = OS_ModuleLoad(&ModuleId, ModuleName, ModuleFile);

    if(ReturnCode != OS_SUCCESS)
    {
        return SBN_ERROR;
    }/* end if */

    ReturnCode = OS_SymbolLookup(&StructAddr, StructName);

    if(ReturnCode != OS_SUCCESS)
    {
        OS_printf("Failed to find symbol %s\n", StructName);
        return SBN_ERROR;
    }/* end if */

    SBN.IfOps[ProtocolId] = (SBN_InterfaceOperations *)StructAddr;

    return SBN_OK;
}/* end SBN_ParseModuleEntry */
#endif /* _osapi_confloader_ */
