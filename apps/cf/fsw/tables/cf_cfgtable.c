/************************************************************************
** File:
**   $Id: cf_cfgtable.c 1.8.1.1 2015/03/06 15:30:28EST sstrege Exp  $
**
**   Copyright © 2007-2014 United States Government as represented by the 
**   Administrator of the National Aeronautics and Space Administration. 
**   All Other Rights Reserved.  
**
**   This software was created at NASA's Goddard Space Flight Center.
**   This software is governed by the NASA Open Source Agreement and may be 
**   used, distributed and modified only pursuant to the terms of that 
**   agreement.
**
** Purpose: 
**  The CFDP (CF) Application Configuration Table Definition
**
** Notes:
**  The configuration parameters in the comments below are defined in the 
**  platform cfg file and dictate the size of the table. 
**  It is required that the 'max' number be defined in the table.
**  Mark unused elements as 'CF_ENTRY_UNUSED'.
**  CF_MAX_PLAYBACK_CHANNELS
**  CF_MAX_POLLING_DIRS_PER_CHAN
**
** $Log: cf_cfgtable.c  $
** Revision 1.8.1.1 2015/03/06 15:30:28EST sstrege 
** Added copyright information
** Revision 1.8 2011/05/09 11:52:12EDT rmcgraw 
** DCR13317:1 Allow Destintaion path to be blank
** Revision 1.7 2010/11/02 10:15:43EDT rmcgraw 
** DCR12802:1 Moved peer entity ID from channels section to polling section
** Revision 1.6 2010/10/25 11:21:50EDT rmcgraw 
** DCR12573:1 Changes to allow more than one incoming PDU MsgId
** Revision 1.5 2010/07/20 14:37:39EDT rmcgraw 
** Dcr11510:1 Remove Downlink buffer references
** Revision 1.4 2010/07/07 17:43:15EDT rmcgraw 
** DCR11510:1 Removed TmpPath
** Revision 1.3 2010/04/23 14:52:47EDT rmcgraw 
** DCR11510:1 Made to match the platform cfg settings
** Revision 1.2 2010/03/12 12:14:34EST rmcgraw 
** DCR11510:1 Initial check-in towards CF Version 1000
** Revision 1.1 2009/11/24 12:49:24EST rmcgraw 
** Initial revision
** Member added to CFS CF project 
**
*************************************************************************/


/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "cfe_tbl_filedef.h"
#include "cf_tbldefs.h"
#include "cf_platform_cfg.h"
#include "cf_msgids.h"
#include "cf_defs.h"

static CFE_TBL_FileDef_t CFE_TBL_FileDef =
{
    "CF_ConfigTable", "CF.ConfigTable", "CF Config Tbl",
    "cf_cfgtable.tbl", sizeof (cf_config_table_t)
};


/*
** Default playback table data - See NOTE Above
*/

cf_config_table_t     CF_ConfigTable =
{

    "CF Default Table",/* TableIdString */
    2,      /* TableVersion (integer)   */    
    4,      /* NumEngCyclesPerWakeup    */
    2,      /* NumWakeupsPerQueueChk    */
    4,      /* NumWakeupsPerPollDirChk  */
    100,    /* UplinkHistoryQDepth      */
    0,      /* Reserved1                */
    0,      /* Reserved2                */
    "10",   /* AckTimeout (secs, entered as string) */
    "2",    /* AckLimit (max timeouts, string)      */
    "5",    /* NakTimeout (secs, string)            */
    "3",    /* NakLimit (max timeouts, string)      */
    "20",   /* InactivityTimeout (secs, string)     */
    "200",  /* OutgoingFileChunkSize (bytes, string)*/
    "no",   /* SaveIncompleteFiles (yes,no, string) */
    "0.24", /* Flight EntityId - 2 byte dotted-decimal string eg. "0.255"*/

    { /* Input Channel Array */
    
        { /* Input Channel 0 */
            
            CF_INCOMING_PDU_MID,
            0, /* Output Chan for Class 2 Uplink Responses, ACK-EOF,Nak,Fin etc) */
            0, /* spare */
        
        }, /* end Input Channel 0 */    
    
    }, /* end Input Channel Array */

    {   /* Playback Channel Array */

        {   /* Playback Channel #0 */  
            CF_ENTRY_IN_USE,                /* Playback Channel Entry In Use */
            CF_ENABLED,                     /* Dequeue Enable */
            CF_SPACE_TO_GND_PDU_MID,        /* Space To Gnd PDU MsgId */
            100,                            /* Pending Queue Depth */
            100,                            /* History Queue Depth */
            "TOPBOutputChan0",              /* Playback Channel Name   */
            "CFTOSemId",                    /* Handshake Semaphore Name   */
        
            {   /* Polling Directory Array */
                
                { /* Polling Directory 0 */
                    CF_ENTRY_IN_USE,/* Poll Directory In Use or Not */
                    CF_DISABLED,    /* Enable State */
                    1,              /* Class (1 or 2)*/
                    5,              /* Priority */                  
                    CF_KEEP_FILE, /* Preserve files after successful transfer? */                    
                    0,              /* Reserved1 */
                    0,              /* Reserved2 */
                    0,              /* Reserved3 */
                    "0.23",         /* Gnd EntityId - 2 byte dotted-decimal string eg. "0.255"*/
                    "/cf/ch0poll0/", /* SrcPath, no spaces, fwd slash at end */
                    "cftesting/",    /* DstPath, no spaces */
                },/* End Polling Directory 0 */
                
                { /* Polling Directory 1 */
                    CF_ENTRY_IN_USE,/* Poll Directory In Use or Not */
                    CF_DISABLED,    /* Enable State */
                    1,              /* Class (1 or 2)*/
                    0,              /* Priority */                  
                    CF_KEEP_FILE,   /* Preserve files after successful transfer? */                    
                    0,              /* Reserved1 */
                    0,              /* Reserved2 */
                    0,              /* Reserved3 */
                    "0.23",         /* Gnd EntityId - 2 byte dotted-decimal string eg. "0.255"*/
                    "/cf/ch0poll1/", /* SrcPathname */
                    "/gnd/",         /* DestPathname */
                    
                },/* End Polling Directory 1 */
                
                
                { /* Polling Directory 2 */
                    CF_ENTRY_IN_USE,/* Poll Directory In Use or Not */
                    CF_DISABLED,    /* Enable State */
                    1,              /* Class (1 or 2)*/
                    5,              /* Priority */                  
                    CF_DELETE_FILE, /* Preserve files after successful transfer? */                    
                    0,              /* Reserved1 */
                    0,              /* Reserved2 */
                    0,              /* Reserved3 */
                    "0.23",         /* Gnd EntityId - 2 byte dotted-decimal string eg. "0.255"*/
                    "/cf/ch0poll2/", /* SrcPathname */
                    "cftesting/",    /* DstPathname */
                },/* End Polling Directory 2 */
                
                { /* Polling Directory 3 */
                    CF_ENTRY_IN_USE,/* Poll Directory In Use or Not */
                    CF_DISABLED,    /* Enable State */
                    1,              /* Class (1 or 2)*/
                    0,              /* Priority */                  
                    CF_KEEP_FILE,   /* Preserve files after successful transfer? */                    
                    0,              /* Reserved1 */
                    0,              /* Reserved2 */
                    0,              /* Reserved3 */
                    "0.23",         /* Gnd EntityId - 2 byte dotted-decimal string eg. "0.255"*/
                    "/cf/ch0poll3/", /* SrcPathname */
                    "/gnd/",         /* DestPathname */
                    
                },/* End Polling Directory 3 */
                
                { /* Polling Directory 4 */
                    CF_ENTRY_IN_USE,/* Poll Directory In Use or Not */
                    CF_DISABLED,    /* Enable State */
                    1,              /* Class (1 or 2)*/
                    5,              /* Priority */                  
                    CF_DELETE_FILE, /* Preserve files after successful transfer? */                    
                    0,              /* Reserved1 */
                    0,              /* Reserved2 */
                    0,              /* Reserved3 */
                    "0.23",         /* Gnd EntityId - 2 byte dotted-decimal string eg. "0.255"*/
                    "/cf/ch0poll4/", /* SrcPathname */
                    "cftesting/",    /* DstPathname */
                },/* End Polling Directory 4 */
                
                { /* Polling Directory 5 */
                    CF_ENTRY_IN_USE,/* Poll Directory In Use or Not */
                    CF_DISABLED,    /* Enable State */
                    1,              /* Class (1 or 2)*/
                    0,              /* Priority */                  
                    CF_KEEP_FILE,   /* Preserve files after successful transfer? */                    
                    0,              /* Reserved1 */
                    0,              /* Reserved2 */
                    0,              /* Reserved3 */
                    "0.23",         /* Gnd EntityId - 2 byte dotted-decimal string eg. "0.255"*/
                    "/cf/ch0poll5/", /* SrcPathname */
                    "/gnd/",         /* DestPathname */
                    
                },/* End Polling Directory 5 */
                
                { /* Polling Directory 6 */
                    CF_ENTRY_IN_USE,/* Poll Directory In Use or Not */
                    CF_DISABLED,    /* Enable State */
                    1,              /* Class (1 or 2)*/
                    5,              /* Priority */                  
                    CF_DELETE_FILE, /* Preserve files after successful transfer? */                    
                    0,              /* Reserved1 */
                    0,              /* Reserved2 */
                    0,              /* Reserved3 */
                    "0.23",         /* Gnd EntityId - 2 byte dotted-decimal string eg. "0.255"*/
                    "/cf/ch0poll6/", /* SrcPathname */
                    "cftesting/",    /* DstPathname */
                },/* End Polling Directory 6 */
                
                { /* Polling Directory 7 */
                    CF_ENTRY_IN_USE,/* Poll Directory In Use or Not */
                    CF_DISABLED,    /* Enable State */
                    1,              /* Class (1 or 2)*/
                    0,              /* Priority */                  
                    CF_KEEP_FILE,   /* Preserve files after successful transfer? */                    
                    0,              /* Reserved1 */
                    0,              /* Reserved2 */
                    0,              /* Reserved3 */
                    "0.23",         /* Gnd EntityId - 2 byte dotted-decimal string eg. "0.255"*/
                    "/cf/ch0poll7/", /* SrcPathname */
                    "/gnd/",         /* DestPathname */
                    
                },/* End Polling Directory 7 */                
                
            }, /* End Polling Directory Array */
        
        },  /* End Playback Channel #0 */
        

        {   /* Playback Channel #1 */  
            CF_ENTRY_UNUSED,                /* Playback Channel Entry In Use */
            CF_DISABLED,                    /* Dequeue Enable */
            CF_SPACE_TO_GND_PDU_MID,        /* Space To Gnd PDU MsgId */
            100,                            /* Pending Queue Depth */
            100,                            /* History Queue Depth */
            "TOPBOutputChan0",              /* Playback Channel Name   */
            "CFTOSemId",                    /* Handshake Semaphore Name   */
        
            {   /* Polling Directory Array */
                
                { /* Polling Directory 0 */
                    CF_ENTRY_UNUSED,/* Poll Directory In Use or Not */
                    CF_DISABLED,    /* Enable State */
                    1,              /* Class (1 or 2)*/
                    0,              /* Priority */                  
                    CF_DELETE_FILE, /* Preserve files after successful transfer? */                    
                    0,              /* Reserved1 */
                    0,              /* Reserved2 */
                    0,              /* Reserved3 */
                    "0.23",         /* Gnd EntityId - 2 byte dotted-decimal string eg. "0.255"*/
                    "/cf/ch1poll0/", /* SrcPathname */
                    "cftesting/",    /* DstPathname */
                },/* End Polling Directory 0 */
                
                { /* Polling Directory 1 */
                    CF_ENTRY_UNUSED,/* Poll Directory In Use or Not */
                    CF_DISABLED,    /* Enable State */
                    1,              /* Class (1 or 2)*/
                    0,              /* Priority */                  
                    CF_DELETE_FILE, /* Preserve files after successful transfer? */                    
                    0,              /* Reserved1 */
                    0,              /* Reserved2 */
                    0,              /* Reserved3 */
                    "0.23",         /* Gnd EntityId - 2 byte dotted-decimal string eg. "0.255"*/
                    "/cf/ch1poll1/", /* SrcPathname */
                    "cftesting/",    /* DstPathname */
                }, /* End Polling Directory 1 */
                
                { /* Polling Directory 2 */
                    CF_ENTRY_UNUSED,/* Poll Directory In Use or Not */
                    CF_DISABLED,    /* Enable State */
                    1,              /* Class (1 or 2)*/
                    0,              /* Priority */                  
                    CF_DELETE_FILE, /* Preserve files after successful transfer? */                    
                    0,              /* Reserved1 */
                    0,              /* Reserved2 */
                    0,              /* Reserved3 */
                    "0.23",         /* Gnd EntityId - 2 byte dotted-decimal string eg. "0.255"*/
                    "/cf/ch1poll2/", /* SrcPathname */
                    "cftesting/",    /* DstPathname */
                },/* End Polling Directory 2 */
                
                { /* Polling Directory 3 */
                    CF_ENTRY_UNUSED,/* Poll Directory In Use or Not */
                    CF_DISABLED,    /* Enable State */
                    1,              /* Class (1 or 2)*/
                    0,              /* Priority */                  
                    CF_DELETE_FILE, /* Preserve files after successful transfer? */                    
                    0,              /* Reserved1 */
                    0,              /* Reserved2 */
                    0,              /* Reserved3 */
                    "0.23",         /* Gnd EntityId - 2 byte dotted-decimal string eg. "0.255"*/
                    "/cf/ch1poll3/", /* SrcPathname */
                    "cftesting/",    /* DstPathname */
                }, /* End Polling Directory 3 */
                
                { /* Polling Directory 4 */
                    CF_ENTRY_UNUSED,/* Poll Directory In Use or Not */
                    CF_DISABLED,    /* Enable State */
                    1,              /* Class (1 or 2)*/
                    0,              /* Priority */                  
                    CF_DELETE_FILE, /* Preserve files after successful transfer? */                    
                    0,              /* Reserved1 */
                    0,              /* Reserved2 */
                    0,              /* Reserved3 */
                    "0.23",         /* Gnd EntityId - 2 byte dotted-decimal string eg. "0.255"*/
                    "/cf/ch1poll4/", /* SrcPathname */
                    "cftesting/",    /* DstPathname */
                },/* End Polling Directory 4 */
                
                { /* Polling Directory 5 */
                    CF_ENTRY_UNUSED,/* Poll Directory In Use or Not */
                    CF_DISABLED,    /* Enable State */
                    1,              /* Class (1 or 2)*/
                    0,              /* Priority */                  
                    CF_DELETE_FILE, /* Preserve files after successful transfer? */                    
                    0,              /* Reserved1 */
                    0,              /* Reserved2 */
                    0,              /* Reserved3 */
                    "0.23",         /* Gnd EntityId - 2 byte dotted-decimal string eg. "0.255"*/
                    "/cf/ch1poll5/", /* SrcPathname */
                    "cftesting/",    /* DstPathname */
                }, /* End Polling Directory 5 */
                
                { /* Polling Directory 6 */
                    CF_ENTRY_UNUSED,/* Poll Directory In Use or Not */
                    CF_DISABLED,    /* Enable State */
                    1,              /* Class (1 or 2)*/
                    0,              /* Priority */                  
                    CF_DELETE_FILE, /* Preserve files after successful transfer? */                    
                    0,              /* Reserved1 */
                    0,              /* Reserved2 */
                    0,              /* Reserved3 */
                    "0.23",         /* Gnd EntityId - 2 byte dotted-decimal string eg. "0.255"*/
                    "/cf/ch1poll6/", /* SrcPathname */
                    "cftesting/",    /* DstPathname */
                },/* End Polling Directory 6 */
                
                { /* Polling Directory 7 */
                    CF_ENTRY_UNUSED,/* Poll Directory In Use or Not */
                    CF_DISABLED,    /* Enable State */
                    1,              /* Class (1 or 2)*/
                    0,              /* Priority */                  
                    CF_DELETE_FILE, /* Preserve files after successful transfer? */                    
                    0,              /* Reserved1 */
                    0,              /* Reserved2 */
                    0,              /* Reserved3 */
                    "0.23",         /* Gnd EntityId - 2 byte dotted-decimal string eg. "0.255"*/
                    "/cf/ch1poll7/", /* SrcPathname */
                    "cftesting/",    /* DstPathname */
                }, /* End Polling Directory 7 */                
                
            }, /* End Polling Directory Array */
        
        },  /* End Playback Channel #1 */

    },  /* End Playback Channel Array */

}; /* End CF_ConfigTable */   
    

/************************/
/*  End of File Comment */
/************************/
