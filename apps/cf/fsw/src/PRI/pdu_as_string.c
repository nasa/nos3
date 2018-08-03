/* FILE: pdu_as_string.c
 *
 *  Copyright © 2007-2014 United States Government as represented by the 
 *  Administrator of the National Aeronautics and Space Administration. 
 *  All Other Rights Reserved.  
 *
 *  This software was created at NASA's Goddard Space Flight Center.
 *  This software is governed by the NASA Open Source Agreement and may be 
 *  used, distributed and modified only pursuant to the terms of that 
 *  agreement.
 *    
 * SPECS: pdu_as_string.h
 * ORIGINAL PROGRAMMER:  Tim Ray 301-286-0581
 * CHANGES:
 *   2007_07_31 Tim Ray
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 */

#include "cfdp.h"
#include "cfdp_private.h"




/*=r=*************************************************************************/
char *ack_struct_as_string (ACK ack)
     /* WHAT IT DOES:  Returns a character-string that summarizes the given
      *   ACK structure.
      */
{
  static char             string [1024];
  char                    substring [32];
  /*------------------------------------------------------------*/

  strcpy (string, "\0");

  /* Include whether the Ack is of an EOF-PDU or a Finished-PDU */
  if (ack.directive_code == EOF_PDU)
    APPEND (string, "Ack-EOF");
  else if (ack.directive_code == FIN_PDU)
    APPEND (string, "Ack-Fin");
  else
    {
      APPEND (string, "INVALID ACK PDU - ");
      APPEND (string, "Directive Code = ");
      sprintf (substring, "%d", ack.directive_code);
      APPEND (string, substring);
      APPEND (string, ".");
      return (string);
    }

  /* Is the PDU being acked effectively a 'Cancel' PDU? -- i.e. an Ack of
   * an EOF (Cancel) or a Finished (Cancel) PDU?
   * Background:  Instead of having a separate 'Cancel' PDU, CFDP Senders
   * use an EOF PDU to cancel, and CFDP Receivers use a Finished PDU to
   * cancel.
   */
  if (ack.condition_code != NO_ERROR)
    APPEND (string, "-Cancel");
  APPEND (string, ":  ");


  /* The Directive Subtype only applies to multi-hop CFDP, so ignore it */
  ;

  /* Include the Transaction Status */
  if (ack.transaction_status == 0)
    APPEND (string, " (undefined transaction)");
  else if (ack.transaction_status == 2)
    APPEND (string, " (terminated transaction)");
  else if (ack.transaction_status == 3)
    APPEND (string, " (unrecognized transaction)");

  APPEND (string, ".");
  return (string);
}



/*=r=*************************************************************************/
char *eof_struct_as_string (EOHF eof)
     /* WHAT IT DOES:  Returns a character-string that summarizes the given
      *   EOHF structure.
      */
{
  static char             string [1024];
  char                    substring [32];
  /*------------------------------------------------------------*/

  strcpy (string, "\0");

  /* Is this a nominal EOF PDU or an EOF-Cancel PDU? */
  if (eof.condition_code == NO_ERROR)
    APPEND (string, "EOF: ");
  else
    {
      APPEND (string, "EOF-Cancel (");
      APPEND (string, cfdp_condition_as_string (eof.condition_code));
      APPEND (string, "):  ");
    }

  /* Include the Checksum */
  APPEND (string, "xsum=");
  sprintf (substring, "%8.8lx", eof.file_checksum);
  APPEND (string, substring);

  /* Include the File-Size */
  APPEND (string, ", file-size=");
  sprintf (substring, "%lu", eof.file_size);
  APPEND (string, substring);

  APPEND (string, ".");
  return (string);
}



/*=r=*************************************************************************/
char *fd_struct_as_string (FD *fd)
     /* WHAT IT DOES:  Returns a character-string that summarizes the given
      *   FD structure.
      */
{
  static char             string [1024];
  char                    substring [32];
  /*------------------------------------------------------------*/
  
  strcpy (string, "FD: ");

  /* Include the Offset */
  APPEND (string, "offset=");
  sprintf (substring, "%lu", fd->offset);
  APPEND (string, substring);

  /* Include the Buffer Length */
  APPEND (string, ", length=");
  sprintf (substring, "%u", fd->buffer_length);
  APPEND (string, substring);
  
  APPEND (string, ".");
  return (string);
}



/*=r=*************************************************************************/
char *fin_struct_as_string (FIN fin)
     /* WHAT IT DOES:  Returns a character-string that summarizes the given
      *   FIN structure.
      */
{
  static char             string [1024];
  /*------------------------------------------------------------*/

  strcpy (string, "\0");

  /* Is this a nominal Finished PDU or a Finished-Cancel PDU? */
  if (fin.condition_code == NO_ERROR)
    APPEND (string, "Fin:  ");
  else
    {
      APPEND (string, "Fin-Cancel (");
      APPEND (string, cfdp_condition_as_string (fin.condition_code));
      APPEND (string, "):  ");
    }

  /* The End System Status only applies for multi-hop CFDP, so ignore it */
  ;

  /* Don't bother including the nominal value for Delivery Code (Data
   * Complete), but do include the error value (Data Incomplete).
   */
  if (fin.delivery_code == 1)
    APPEND (string, "(incomplete delivery)");
  
  APPEND (string, ".");
  return (string);
}



/*=r=*************************************************************************/
char *md_struct_as_string (MD md)
     /* WHAT IT DOES:  Returns a character-string that summarizes the given
      *   MD structure.
      */
{
  static char             string [1024];
   /*------------------------------------------------------------*/
  
  strcpy (string, "MD:  ");

  /* Include the Source File Name */
  APPEND (string, "'");
  APPEND (string, md.source_file_name);
  APPEND (string, "'");

  /* Include the Destination File Name if it is different than Source File */
  if (strncmp (md.source_file_name, md.dest_file_name, MAX_FILE_NAME_LENGTH))
    {
      APPEND (string, " -> '");
      APPEND (string, md.dest_file_name);
      APPEND (string, "'");
    }
  
  APPEND (string, ".");
  return (string);
}



/*=r=*************************************************************************/
char *nak_struct_as_string (NAK nak)
     /* WHAT IT DOES:  Returns a character-string that summarizes the given
      *   NAK structure.
      */
{
  static char             string [1024];
   /*------------------------------------------------------------*/
  
  strcpy (string, "Nak:  ");

  /* Include the list of gaps */
  APPEND (string, nak__list_as_string (&nak));

  APPEND (string, ".");
  return (string);
}



/*=r=*************************************************************************/
char *pdu_struct_as_string (PDU_AS_STRUCT pdu_as_struct)
     /* WHAT IT DOES:  Returns a char-string that summarizes the given PDU. */
{
  static char             string [1024];
  char                    substring [32];
   /*------------------------------------------------------------*/

  /* Include the PDU's transaction-id */
  strcpy (string, "(");
  APPEND (string, cfdp_trans_as_string (pdu_as_struct.hdr.trans));
  APPEND (string, ") ");

  /* Include the appropriate info for each PDU-type */
  if (pdu_as_struct.is_this_a_file_data_pdu)
    APPEND (string, fd_struct_as_string (&pdu_as_struct.data_field.fd));
  else if (pdu_as_struct.dir_code == MD_PDU)
    APPEND (string, md_struct_as_string (pdu_as_struct.data_field.md));
  else if (pdu_as_struct.dir_code == EOF_PDU)
    APPEND (string, eof_struct_as_string (pdu_as_struct.data_field.eof));
  else if (pdu_as_struct.dir_code == FIN_PDU)
    APPEND (string, fin_struct_as_string (pdu_as_struct.data_field.fin));
  else if (pdu_as_struct.dir_code == ACK_PDU)
    APPEND (string, ack_struct_as_string (pdu_as_struct.data_field.ack));
  else if (pdu_as_struct.dir_code == NAK_PDU)
    APPEND (string, nak_struct_as_string (pdu_as_struct.data_field.nak));
  else
    {
      APPEND (string, "(unrecognized Directive Code = ");
      sprintf (substring, "%d", pdu_as_struct.dir_code);
      APPEND (string, substring);
      APPEND (string, ").");
    }

  return (string);
}
