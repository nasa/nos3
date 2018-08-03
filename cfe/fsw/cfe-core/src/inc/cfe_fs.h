/*
** $Id: cfe_fs.h 1.4 2010/10/25 16:50:16GMT-05:00 jmdagost Exp  $
**
** Purpose:  cFE File Services (FS) library API header file
**
** Author:   S.Walling/Microtel
**
** $Log: cfe_fs.h  $
** Revision 1.4 2010/10/25 16:50:16GMT-05:00 jmdagost 
** Added assumption to CFE_FS_ExtractFilenameFromPath()
** Revision 1.3 2008/08/28 08:41:10EDT apcudmore 
** Fixed CFS name ( CFS-->CFE )
** Revision 1.2 2008/06/20 15:40:26EDT apcudmore 
** Added support for OSAL based module loader
**  - Removed calls and references to the BSP based module loader
** Revision 1.1 2008/04/17 08:05:22EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/inc/project.pj
** Revision 1.15 2007/09/20 14:47:24EDT apcudmore 
** Changed CFE_FS_SB_QUERYALLTASKS_SUBTYPE back to CFE_FS_ES_QUERYALLTASKS_SUBTYPE
** Revision 1.14 2007/09/20 13:17:52EDT David Kobe (dlkobe) 
** Added documentation for each File SubType identifier
** Revision 1.13 2007/09/20 10:52:52EDT apcudmore 
** Added Query All Tasks command, file definition, events, error codes etc.
** Revision 1.12 2007/05/23 11:21:57EDT dlkobe 
** Added doxygen formatting
** Revision 1.11 2007/05/09 12:43:03EDT dlkobe 
** Added CFE_FS_ES_CDS_REG_SUBTYPE sub type to support Dump CDS Registry Command
** Revision 1.10 2007/03/29 08:58:09EST rjmcgraw 
** Added shell subtype define
** Revision 1.9 2007/03/28 15:18:24EST rjmcgraw 
** Moved file related defines from cfe_tbl.h to cfe_fs.h
** Revision 1.8 2007/03/16 15:12:13EST dlkobe 
** Swapped parameter list for CFE_FS_ReadHeader API.
** Revision 1.7 2006/11/17 16:46:11GMT-05:00 dlkobe 
** Updated with Doxygen formatted comments
** Revision 1.6 2006/07/25 14:37:50GMT-05:00 njyanchik 
** It turns out the both the FS app and the OSAL were incorrect where file 
** descriptors are concerned. File descriptors should be int32 across the board.
**
*/

/*
** Ensure that header is included only once...
*/
#ifndef _cfe_fs_
#define _cfe_fs_


/*
** Required header files...
*/
#include "common_types.h"
#include "cfe_time.h"



/******************* Macro Definitions ***********************/
#define CFE_FS_HDR_DESC_MAX_LEN 32 /**< \brief Max length of description field in a standard cFE File Header */

#define CFE_FS_FILE_CONTENT_ID  0x63464531  /**< \brief Magic Number for cFE compliant files (= 'cFE1') */

/** \name Standard cFE File Subtype Identifiers */
/** \{ */
#define CFE_FS_ES_ERLOG_SUBTYPE         1     /**< \brief Executive Services Exception/Reset Log Type */
                                              /**< Executive Services Exception/Reset Log File 
                                                   which is generated in response to a 
                                                   \link #CFE_ES_WRITE_ERLOG_CC \ES_WRITEERLOG2FILE \endlink
                                                   command. */
#define CFE_FS_ES_SYSLOG_SUBTYPE        2     /**< \brief Executive Services System Log Type */
                                              /**< Executive Services System Log File which is 
                                                   generated in response to a 
                                                   \link #CFE_ES_WRITE_SYSLOG_CC \ES_WRITESYSLOG2FILE \endlink
                                                   command. */
#define CFE_FS_ES_QUERYALL_SUBTYPE      3     /**< \brief Executive Services Information on All Applications File */
                                              /**< Executive Services Information on All 
                                                   Applications File which is generated in response to a 
                                                   \link #CFE_ES_QUERY_ALL_CC \ES_WRITEAPPINFO2FILE \endlink
                                                   command. */
#define CFE_FS_ES_PERFDATA_SUBTYPE      4     /**< \brief Executive Services Performance Data File */
                                              /**< Executive Services Performance Analyzer Data File 
                                                   which is generated in response to a 
                                                   \link #CFE_ES_PERF_STOPDATA_CC \ES_STOPLADATA \endlink
                                                   command. */
#define CFE_FS_ES_SHELL_SUBTYPE         5     /**< \brief Executive Services Shell Response File */
                                              /**< Executive Services Shell Response Data File 
                                                   which is generated in response to a 
                                                   \link #CFE_ES_SHELL_CMD_CC \ES_SHELL \endlink
                                                   command. */
#define CFE_FS_ES_CDS_REG_SUBTYPE       6     /**< \brief Executive Services Critical Data Store Registry Dump File */
                                              /**< Executive Services Critical Data Store Registry Dump File 
                                                   which is generated in response to a 
                                                   \link #CFE_ES_DUMP_CDS_REG_CC \ES_DUMPCDSREG \endlink
                                                   command. */
#define CFE_FS_TBL_REG_SUBTYPE          9     /**  \brief Table Services Registry Dump File */
                                              /**< Table Services Registry Dump File 
                                                   which is generated in response to a 
                                                   \link #CFE_TBL_DUMP_REG_CC \TBL_WRITEREG2FILE \endlink
                                                   command. */
#define CFE_FS_TBL_IMG_SUBTYPE          8     /**< \brief Table Services Table Image File */
                                              /**< Table Services Table Image File 
                                                   which is generated either on the ground or in response to a 
                                                   \link #CFE_TBL_DUMP_CC \TBL_DUMP \endlink command. */
#define CFE_FS_EVS_APPDATA_SUBTYPE      15    /**< \brief Event Services Application Data Dump File */
                                              /**< Event Services Application Data Dump File 
                                                   which is generated in response to a 
                                                   \link #CFE_EVS_FILE_WRITE_APP_DATA_CC \EVS_WRITEAPPDATA2FILE \endlink
                                                   command. */
#define CFE_FS_EVS_EVENTLOG_SUBTYPE     16    /**< \brief Event Services Local Event Log Dump File */
                                              /**< Event Services Local Event Log Dump File 
                                                   which is generated in response to a 
                                                   \link #CFE_EVS_FILE_WRITE_LOG_DATA_CC \EVS_WRITELOG2FILE \endlink
                                                   command. */
#define CFE_FS_SB_PIPEDATA_SUBTYPE      20    /**< \brief Software Bus Pipe Data Dump File */
                                              /**< Software Bus Pipe Data Dump File 
                                                   which is generated in response to a 
                                                   \link #CFE_SB_SEND_PIPE_INFO_CC \SB_WRITEPIPE2FILE \endlink
                                                   command. */
#define CFE_FS_SB_ROUTEDATA_SUBTYPE     21    /**< \brief Software Bus Message Routing Data Dump File */
                                              /**< Software Bus Message Routing Data Dump File 
                                                   which is generated in response to a 
                                                   \link #CFE_SB_SEND_ROUTING_INFO_CC \SB_WRITEROUTING2FILE \endlink
                                                   command. */
#define CFE_FS_SB_MAPDATA_SUBTYPE       22    /**< \brief Software Bus Message Mapping Data Dump File */
                                              /**< Software Bus Message Mapping Data Dump File 
                                                   which is generated in response to a 
                                                   \link #CFE_SB_SEND_MAP_INFO_CC \SB_WRITEMAP2FILE \endlink
                                                   command. */
#define CFE_FS_ES_QUERYALLTASKS_SUBTYPE 23    /**< \brief Executive Services Query All Tasks Data File */
                                              /**< Executive Services Query All Tasks Data File 
                                                   which is generated in response to a 
                                                   \link #CFE_ES_QUERY_ALL_TASKS_CC \ES_WRITETASKINFO2FILE \endlink
                                                   command. */
/** \} */

/**
** \brief Standard cFE File header structure definition
*/
typedef struct
{
    uint32  ContentType;           /**< \brief Identifies the content type (='cFE1'=0x63464531)*/
    uint32  SubType;               /**< \brief Type of \c ContentType, if necessary */
                                   /**< Standard SubType definitions can be found
                                        \link #CFE_FS_ES_ERLOG_SUBTYPE here \endlink */
    uint32  Length;                /**< \brief Length of primary header */
    uint32  SpacecraftID;          /**< \brief Spacecraft that generated the file */
    uint32  ProcessorID;           /**< \brief Processor that generated the file */
    uint32  ApplicationID;         /**< \brief Application that generated the file */
  
    uint32  TimeSeconds;           /**< \brief File creation timestamp (seconds) */
    uint32  TimeSubSeconds;        /**< \brief File creation timestamp (sub-seconds) */

    char    Description[CFE_FS_HDR_DESC_MAX_LEN];       /**< \brief File description */

} CFE_FS_Header_t;


/*
** File header access functions...
*/

/*****************************************************************************/
/**
** \brief Read the contents of the Standard cFE File Header
**
** \par Description
**        This API will fill the specified #CFE_FS_Header_t variable with the
**        contents of the Standard cFE File Header of the file identified by
**        the given File Descriptor.
**
** \par Assumptions, External Events, and Notes:
**        -# The File has already been successfully opened using #OS_open and
**           the caller has a legitimate File Descriptor.
**
** \param[in] FileDes File Descriptor obtained from a previous call to #OS_open
**                    that is associated with the file whose header is to be read.
**
** \param[in] Hdr     Pointer to a variable of type #CFE_FS_Header_t that will be
**                    filled with the contents of the Standard cFE File Header.
**
** \param[out] *Hdr   Contents of the Standard cFE File Header for the specified file.
**
** \returns
** \retstmt Any of the return codes specified for #OS_lseek or #OS_read  \endstmt
** \endreturns
**
** \sa #CFE_FS_WriteHeader
**
******************************************************************************/
int32 CFE_FS_ReadHeader(CFE_FS_Header_t *Hdr, int32 FileDes);

/*****************************************************************************/
/**
** \brief Initializes the contents of the Standard cFE File Header
**
** \par Description
**        This API will clear the specified #CFE_FS_Header_t variable and
**        initialize the description field with the specified value
**
** \param[in] Hdr     Pointer to a variable of type #CFE_FS_Header_t that will be
**                    cleared and initialized
**
** \sa #CFE_FS_WriteHeader
**
******************************************************************************/
void CFE_FS_InitHeader(CFE_FS_Header_t *Hdr, const char *Description, uint32 SubType);

/*****************************************************************************/
/**
** \brief Write the specified Standard cFE File Header to the specified file
**
** \par Description
**        This API will output the specified #CFE_FS_Header_t variable, with some
**        fields automatically updated, to the specified file as the Standard cFE
**        File Header. This API will automatically populate the following fields
**        in the specified #CFE_FS_Header_t:
**
**      -# \link #CFE_FS_Header_t::ContentType \c ContentType \endlink - Filled with 0x63464531 ('cFE1')
**      -# \link #CFE_FS_Header_t::Length \c Length \endlink - Filled with the sizeof(CFE_FS_Header_t)
**      -# \link #CFE_FS_Header_t::SpacecraftID \c SpacecraftID \endlink - Filled with the Spacecraft ID
**      -# \link #CFE_FS_Header_t::ProcessorID \c ProcessorID \endlink - Filled with the Processor ID
**      -# \link #CFE_FS_Header_t::ApplicationID \c ApplicationID \endlink -  Filled with the Application ID
**      -# \link #CFE_FS_Header_t::TimeSeconds \c TimeSeconds \endlink - Filled with the Time, in seconds, as obtained by #CFE_TIME_GetTime
**      -# \link #CFE_FS_Header_t::TimeSubSeconds \c TimeSubSeconds \endlink - Filled with the Time, subseconds, as obtained by #CFE_TIME_GetTime
**        
**        
** \par Assumptions, External Events, and Notes:
**        -# The File has already been successfully opened using #OS_open and
**           the caller has a legitimate File Descriptor.
**        -# The \c SubType field has been filled appropriately by the Application.
**        -# The \c Description field has been filled appropriately by the Application.
**
** \param[in] FileDes File Descriptor obtained from a previous call to #OS_open
**                    that is associated with the file whose header is to be read.
**
** \param[in] Hdr     Pointer to a variable of type #CFE_FS_Header_t that will be
**                    filled with the contents of the Standard cFE File Header.
**
** \param[out] *Hdr   Contents of the Standard cFE File Header for the specified file.
**
** \returns
** \retstmt Any of the return codes specified for #OS_lseek or #OS_write       \endstmt
** \endreturns
**
** \sa #CFE_FS_ReadHeader
**
******************************************************************************/
int32 CFE_FS_WriteHeader(int32 FileDes, CFE_FS_Header_t *Hdr);

/*****************************************************************************/
/**
** \brief Modifies the Time Stamp field in the Standard cFE File Header for the specified file
**
** \par Description
**        This API will modify the \link #CFE_FS_Header_t::TimeSeconds timestamp \endlink found
**        in the Standard cFE File Header of the specified file.  The timestamp will be replaced
**        with the time specified by the caller.
**
** \par Assumptions, External Events, and Notes:
**        -# The File has already been successfully opened using #OS_open and
**           the caller has a legitimate File Descriptor.
**        -# The \c NewTimestamp field has been filled appropriately by the Application.
**
** \param[in] FileDes File Descriptor obtained from a previous call to #OS_open
**                    that is associated with the file whose header is to be read.
**
** \param[in] NewTimestamp A #CFE_TIME_SysTime_t data structure containing the desired time
**                         to be put into the file's Standard cFE File Header.
**
** \returns
** \retstmt Any of the return codes specified for #OS_lseek or #OS_write   \endstmt
** \endreturns
**
** \sa
**               
******************************************************************************/
int32 CFE_FS_SetTimestamp(int32 FileDes, CFE_TIME_SysTime_t NewTimestamp);


/*****************************************************************************/
/**
** \brief Determines if a file is a Gzip/compressed file.
**
** \par Description
**        This API will check the filename and return TRUE if the file is 
**        a gzip file. The check is currently based on the filename, so the 
**        zipped files should use the ".gz" extention. 
**
** \par Assumptions, External Events, and Notes:
**        -# A gzipped file will use the ".gz" filename extention.
**
** \param[in] FileName The name of the file.
**
** \returns
** \retstmt TRUE if the file has the ".gz" extention and FALSE otherwise.   \endstmt
** \endreturns
**
** \sa
**               
******************************************************************************/
boolean CFE_FS_IsGzFile(const char *FileName);

/*****************************************************************************/
/**
** \brief Extracts the filename from a unix style path and filename string.
**
** \par Description
**        This API will take the original unix path/filename combination and 
**        extract the base filename. Example: Given the path/filename : "/cf/apps/myapp.o.gz"
**        this function will return the filename: "myapp.o.gz".
**
** \par Assumptions, External Events, and Notes:
**        -# The paths and filenames used here are the standard unix style 
**            filenames separated by "/" characters.
**        -# The extracted filename is no longer than #OS_MAX_PATH_LEN 
**
** \param[in] OriginalPath The original path.
** \param[out] FileNameOnly The filename that is extracted from the path.
**
** \returns
** \retstmt CFE_SUCCESS if the filename was extracted from the path  \endstmt
** \endreturns
**
** \sa
**               
******************************************************************************/
int32 CFE_FS_ExtractFilenameFromPath(const char *OriginalPath, char *FileNameOnly);

/*****************************************************************************/
/**
** \brief Decompresses the source file to the destination file.
**
** \par Description
**        This API will decompress the source file to the file specified by
**        the destination file. The file must be compressed using the "gzip"
**        utility. This utility is available on most unix workstations, Mac OS X, 
**        Cygwin, and MinGW for Windows. More information can be found at
**         http://www.gzip.org/
**
** \par Assumptions, External Events, and Notes:
**        -# The paths and filenames used here are cfe compliant file names.
**        -# The source file is compressed with the "gzip" utility.
**        -# The destination file does not exist, or can be overwritten. 
**
** \param[in] SourceFile The "gzipped" file to decompress.
** \param[out] DestinationFile The path/filename to write the decompressed or
**             "gunzipped" file to.
**
** \returns
** \retstmt CFE_SUCCESS if the file was decompressed sucessfully.  \endstmt
** \endreturns
**
** \sa
**               
******************************************************************************/
int32 CFE_FS_Decompress( const char * SourceFile, const char * DestinationFile );


#endif /* _cfe_fs_ */

/************************/
/*  End of File Comment */
/************************/
