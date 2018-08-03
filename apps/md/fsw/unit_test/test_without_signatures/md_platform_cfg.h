/*************************************************************************
** File:
**   $Id: md_platform_cfg.h 1.2 2012/01/09 19:28:19EST aschoeni Exp  $
**
** Purpose: 
**   Specification for the CFS Memory Dwell constants that can
**   be configured from one platform to another
**
** Notes:
**
**   $Log: md_platform_cfg.h  $
**   Revision 1.2 2012/01/09 19:28:19EST aschoeni 
**   Updated to support newest versions
**   Revision 1.6 2009/10/02 19:12:51EDT aschoeni 
**   Updated verify.h and platform config file.
**   Revision 1.5 2009/09/30 14:14:44EDT aschoeni 
**   Added note about signature length.
**   Revision 1.4 2008/10/06 10:29:51EDT dkobe 
**   Updated and Corrected Doxygen Comments
**   Revision 1.3 2008/09/05 17:43:56EDT nsschweiss 
**   Deleted perf id.
**   Revision 1.2 2008/07/02 13:34:55EDT nsschweiss 
**   CFS MD Post Code Review Version
**   Date: 08/05/09
**   CPID: 1653:2
** 
*************************************************************************/

#ifndef _md_platform_cfg_h_
#define _md_platform_cfg_h_


/*************************************************************************
** Macro Definitions
*************************************************************************/

/**
** \name MD Command Pipe Parameters */ 
/** \{ */
#define MD_PIPE_NAME       "MD_CMD_PIPE"
#define MD_PIPE_DEPTH      50
/** \} */

/**
**  \mdcfg Number of memory dwell tables.
**
**  \par Limits
**       Acceptable values for this parameter are 1 to 16.
*/
#define MD_NUM_DWELL_TABLES     4

/**
**  \mdcfg Maximum number of dwell specifications (address/delay/length) 
**    in a Dwell Table.
**
**  \par Limits
**       The maximum value for this parameter is limited by its effect
**       on the MD_DwellTableLoad_t and associated data points, limiting
**       it to 65535 points.  
*/
#define MD_DWELL_TABLE_SIZE 25    

/**
**  \mdcfg Option of whether 32 bit integers must be aligned to 32 bit
**   boundaries.  1 indicates 'yes' (32-bit boundary alignment enforced),
**   0 indicates 'no' (16-bit boundary alignment enforced).
**
**  \par Limits
**       Value must be 0 or 1.
*/
#define MD_ENFORCE_DWORD_ALIGN 0

/**
**  \mdcfg Option of whether a signature field will be reserved in 
**     dwell packets.  1 indicates 'yes', 0 indicates 'no'.
**
**  \par Limits
**       Value must be 0 or 1.
*/
#define MD_SIGNATURE_OPTION 0

/**
**  \mdcfg Number of characters used in the Signature Field
**
**  \par Limits
**       Signature field length needs to be a multiple of 4 so that
**       dwell packet is a multiple of 4 bytes and no compiler padding 
**       will occur. Note that the final character of the signature string
**       must be a null character, so the effective length of user definable
**       characters is one less than the defined length. The length 
**       specified must therefore be at least 4.
*/
#define MD_SIGNATURE_FIELD_LENGTH 32

#endif /* _md_platform_cfg_ */

/************************/
/*  End of File Comment */
/************************/
