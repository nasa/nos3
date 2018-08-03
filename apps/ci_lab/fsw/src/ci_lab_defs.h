/*******************************************************************************
** File:
**   $Id: ci_lab_defs.h 1.1 2010/09/08 07:05:25GMT-05:00 wmoleski Exp  $
**
** Purpose: 
**  Define CI Lab Definitions that other apps may need to use
**
** Notes:
**
** $Log: ci_lab_defs.h  $
** Revision 1.1 2010/09/08 07:05:25GMT-05:00 wmoleski 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/ci_lab/fsw/src/project.pj
**
*******************************************************************************/
#ifndef _ci_lab_defs_h_
#define _ci_lab_defs_h_

/*
** Definitions
*/
/* File Size Command argument values */
#define PDU_SIZE_ADD		0
#define PDU_SIZE_SUBTRACT	1

/* PDU Type argument values */
#define FILE_DATA_PDU	0
#define EOF_PDU		1
#define FIN_PDU		2
#define ACK_PDU		3
#define META_DATA_PDU	4
#define NAK_PDU		5

#endif /* _ci_lab_defs_h_ */

/************************/
/*  End of File Comment */
/************************/
