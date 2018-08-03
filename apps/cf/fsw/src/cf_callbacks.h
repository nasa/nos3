/************************************************************************
** File:
**   $Id: cf_callbacks.h 1.6 2015/03/06 14:49:56EST sstrege Exp  $
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
**  The CFS CF Application header file for callback related items
**
** Notes:
**
** $Log: cf_callbacks.h  $
** Revision 1.6 2015/03/06 14:49:56EST sstrege 
** Added copyright information
** Revision 1.5 2011/05/17 10:21:46EDT rmcgraw 
** DCR14967:4 Function Register_callbacks cannot return err, check for err removed
** Revision 1.4 2010/07/20 14:37:45EDT rmcgraw 
** Dcr11510:1 Remove Downlink buffer references
** Revision 1.3 2010/03/26 15:30:24EDT rmcgraw 
** DCR11510 Various developmental changes
** Revision 1.2 2010/03/12 12:14:35EST rmcgraw 
** DCR11510:1 Initial check-in towards CF Version 1000
** Revision 1.1 2009/11/24 12:48:51EST rmcgraw 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cf/fsw/src/project.pj
**
*************************************************************************/
#ifndef _cf_callbacks_h_
#define _cf_callbacks_h_


/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "cfdp_data_structures.h"


void CF_RegisterCallbacks(void);

/* Callback for indication function */
void CF_Indication (INDICATION_TYPE IndType, TRANS_STATUS TransInfo);

boolean CF_PduOutputOpen (ID SourceId, ID DestinationId);
/* Called when engine has a PDU ready for outut */
boolean CF_PduOutputReady (PDU_TYPE PduType, TRANSACTION TransInfo,ID DestinationId);
/* Writes PDU to software bus */
void CF_PduOutputSend (TRANSACTION TransInfo, ID DestinationId, CFDP_DATA *PduPtr);

/* Callback functions for printf arguments. */
int CF_DebugEvent (const char *Format, ...);
int CF_InfoEvent (const char *Format, ...);
int CF_WarningEvent (const char *Format, ...);
int CF_ErrorEvent (const char *Format, ...); 


/* Callback functions for Filestore routines */

/*
* These functions are compliant with ANSI and POSIX. 
* Changing "int" to "int32" will produce compiler warnings.
*/
int CF_RenameFile(const char *OldName, const char *NewName);
int CF_RemoveFile(const char *Name);
CFDP_FILE * CF_Fopen(const char *Name, const char *Mode);
u_int_4 CF_FileSize(const char *Name);
int CF_Fseek(CFDP_FILE *File, long int Offset, int Whence);
size_t CF_Fread(void *Buffer, size_t Size,size_t Count, CFDP_FILE *File);
size_t CF_Fwrite(const void *Buff, size_t Size,size_t Count, CFDP_FILE *File);
int CF_Fclose(CFDP_FILE *File);

int32   CF_Tmpcreat  (const char *path, int32  access);
int32   CF_Tmpopen   (const char *path,  int32 access,  uint32 mode);
int32   CF_Tmpclose  (int32  filedes);
int32   CF_Tmpread   (int32  filedes, void *buffer, uint32 nbytes);
int32   CF_Tmpwrite  (int32  filedes, void *buffer, uint32 nbytes);
int32   CF_Tmplseek  (int32  filedes, int32 offset, uint32 whence);

int32 CF_PendingQueueSort(uint8 Channel);

#endif /* _cf_callbacks_ */

/************************/
/*  End of File Comment */
/************************/
