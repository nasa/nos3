/* FILE: default_indication.c   The default implementation of the callback
 *   routine that responds to CFDP Indications.
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
 * SPECS: The CFDP protocol does not specify the response to Indications.
 * ORIGINAL PROGRAMMER:  Tim Ray 301-286-0581
 * CHANGES:
 *   2007_07_31 Tim Ray (TR)
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 *   2007_11_09 TR
 *     - Enhancement and bug fix.  The default message produced in response
 *       to a Transaction-Finished Indication now provides more information
 *       and is more accurate.
 *   2007_12_06 TR
 *   2007_12_06 TR
 *     - Enhancement.  Message-class filtering can now be changed on-the-fly.
 */

#include "cfdp.h"
#include "cfdp_private.h"



/*=r=************************************************************************/
static void m__transaction (const TRANS_STATUS *info)
{
  char             msg [1024];
  char             substring [32];
  /*------------------------------------------------------------*/

  /*-------------------------------------*/
  /* Build an informational text message */
  /*-------------------------------------*/

  strcpy (msg, ":::Trans  ");
  APPEND (msg, cfdp_trans_as_string (info->trans));
  APPEND (msg, ", ");

  if ((info->role == S_1) || (info->role == R_1))
    APPEND (msg, "class 1, ");
  else if ((info->role == S_2) || (info->role == R_2))
    APPEND (msg, "class 2, ");
  else
    APPEND (msg, "class ?, ");
  
  if (info->md.file_transfer)
    {
      APPEND (msg, "sending '");
      APPEND (msg, info->md.source_file_name);
      APPEND (msg, "' to node ");
      APPEND (msg, cfdp_id_as_string (info->partner_id));
      APPEND (msg, " ");

      if (info->md.file_size != 0)
        {
          APPEND (msg, "(");
          sprintf (substring, "%lu", info->md.file_size);
          APPEND (msg, substring);
          APPEND (msg, " bytes).\n");
        }
      else
        APPEND (msg, "(unbounded file).\n");

    }
  else
    APPEND (msg, "No file transfer.\n");

  /*---------------------------*/
  /* Finally, send the message */
  /*---------------------------*/
  i_msg__ (msg);
}


/*=r=************************************************************************/
static void m__abandoned (const TRANS_STATUS *info)
   {
   /*------------------------------------------------------------*/
     i_msg__ (":::Abandoned  trans %s, due to '%s'.\n",
              cfdp_trans_as_string (info->trans),
              cfdp_condition_as_string (info->condition_code));
   }



/*=r=************************************************************************/
static void m__fault (const TRANS_STATUS *info)
   {
   /*------------------------------------------------------------*/
     e_msg__ (":::Fault  trans %s, fault='%s'.\n",
              cfdp_trans_as_string (info->trans),
              cfdp_condition_as_string (info->condition_code));
   }



/*=r=************************************************************************/
static void m__metadata_recv (const TRANS_STATUS *info)
{
  char             msg [1024];
  char             substring [32];
  /*------------------------------------------------------------*/

  /*-------------------------------------*/
  /* Build an informational text message */
  /*-------------------------------------*/

  strcpy (msg, ":::MD_Recv  trans ");
  APPEND (msg, cfdp_trans_as_string (info->trans));
  APPEND (msg, ", ");

  if ((info->role == S_1) || (info->role == R_1))
    APPEND (msg, "class 1, ");
  else if ((info->role == S_2) || (info->role == R_2))
    APPEND (msg, "class 2, ");
  else
    APPEND (msg, "class ?, ");
  
  if (info->md.file_transfer)
    {
      APPEND (msg, "receiving '");
      APPEND (msg, info->md.dest_file_name);
      APPEND (msg, "' ");
      if (info->md.file_size != 0)
        {
          APPEND (msg, "(");
          sprintf (substring, "%lu", info->md.file_size);
          APPEND (msg, substring);
          APPEND (msg, " bytes).\n");
        }
      else
        APPEND (msg, "(unbounded file).\n");
    }
  else
    APPEND (msg, "no file transfer.\n");

  /*---------------------------*/
  /* Finally, send the message */
  /*---------------------------*/
  i_msg__ (msg);
}



/*=r=************************************************************************/
static void m__metadata_sent (const TRANS_STATUS *info)
{
  char             msg [1024];
  char             substring [32];
  /*------------------------------------------------------------*/

  /*-------------------------------------*/
  /* Build an informational text message */
  /*-------------------------------------*/

  strcpy (msg, ":::MD_Sent  trans ");
  APPEND (msg, cfdp_trans_as_string (info->trans));
  APPEND (msg, ", ");

  if ((info->role == S_1) || (info->role == R_1))
    APPEND (msg, "class 1, ");
  else if ((info->role == S_2) || (info->role == R_2))
    APPEND (msg, "class 2, ");
  else
    APPEND (msg, "class ?, ");
  
  if (info->md.file_transfer)
    {
      APPEND (msg, "sending '");
      APPEND (msg, info->md.dest_file_name);
      APPEND (msg, "' ");
      if (info->md.file_size != 0)
        {
          APPEND (msg, "(");
          sprintf (substring, "%lu", info->md.file_size);
          APPEND (msg, substring);
          APPEND (msg, " bytes).\n");
        }
      else
        APPEND (msg, "(unbounded file).\n");
    }
  else
    APPEND (msg, "no file transfer.\n");

  /*---------------------------*/
  /* Finally, send the message */
  /*---------------------------*/
  i_msg__ (msg);
}



/*=r=************************************************************************/
static void m__file_segment_recv (const TRANS_STATUS *info)
   {
   /*------------------------------------------------------------*/
     d_msg__ (":::File_Segment_Recv  trans %s; offset=%lu, length=%lu.\n",
              cfdp_trans_as_string (info->trans),
              info->fd_offset, info->fd_length);
   }



/*=r=************************************************************************/
static void m__file_segment_sent (const TRANS_STATUS *info)
   {
   /*------------------------------------------------------------*/
     d_msg__ (":::File_Segment_Sent  trans %s; offset=%lu, length=%lu.\n",
              cfdp_trans_as_string (info->trans),
              info->fd_offset, info->fd_length);
   }



/*=r=************************************************************************/
static void m__transaction_finished (const TRANS_STATUS *info)
{
  char             msg [1024];
  char             status_string [512];
  /*------------------------------------------------------------*/

  /*-------------------------------------*/
  /* Build an informational text message */
  /*-------------------------------------*/

  /* Message starts with Indication-type and Transaction-ID */
  strcpy (msg, ":::Trans_Finished  trans ");
  APPEND (msg, cfdp_trans_as_string (info->trans));
  APPEND (msg, " ");

  /* Build a substring that summarizes the final status */
  if (info->final_status == FINAL_STATUS_SUCCESSFUL)
    sprintf (status_string, "successful");
  else if (info->final_status == FINAL_STATUS_ABANDONED)
    {
      if (info->condition_code == NO_ERROR)
        sprintf (status_string, "abandoned (User Request)");
      else
        sprintf (status_string, "abandoned (%s)", 
                 cfdp_condition_as_string (info->condition_code));
    }
  else if (info->final_status == FINAL_STATUS_CANCELLED)
    sprintf (status_string, "cancelled (%s)",
             cfdp_condition_as_string (info->condition_code));
  else if (info->final_status == FINAL_STATUS_NO_METADATA)
    sprintf (status_string, "failed (Metadata not received)");
  else if (info->final_status == FINAL_STATUS_UNKNOWN)
    sprintf (status_string, "<PROBLEM> Final Status = Unknown");
  else
    sprintf (status_string, "<PROBLEM> Final Status unrecognized");

  /* Append the substring we just built to the message */
  APPEND (msg, status_string);
  APPEND (msg, ".\n");

  /*---------------------------*/
  /* Finally, send the message */
  /*---------------------------*/
  i_msg__ (msg);
}



/*=r=************************************************************************/
static void m__report (const TRANS_STATUS *info)
{
  char             msg [1024];
  char             substring [32];
  char             susp_or_frozen [16];
  /*------------------------------------------------------------*/

  /*-------------------------------------*/
  /* Build an informational text message */
  /*-------------------------------------*/

  strcpy (msg,":::Report  ");

  APPEND (msg, "Role=");
  APPEND (msg, cfdp_role_as_string (info->role));
  APPEND (msg, ", ");

  APPEND (msg, "State=S");
  sprintf (substring, "%u", info->state);
  APPEND (msg, substring);
  APPEND (msg, ", ");

  strcpy (susp_or_frozen, "\0");
  if (info->frozen)
    strcat (susp_or_frozen, "F");
  if (info->suspended)
    strcat (susp_or_frozen, "S");
  if (strlen (susp_or_frozen) > 0)
    {
      APPEND (msg, "(");
      APPEND (msg, susp_or_frozen);
      APPEND (msg, ") ");
    }

  APPEND (msg, "Trans=");
  APPEND (msg, cfdp_trans_as_string (info->trans));
  APPEND (msg, ", ");

  if ((info->role == S_1) || (info->role == S_2))
    {
      if (info->md.file_transfer)
        {
          sprintf (substring, "%lu", info->fd_offset);
          APPEND (msg, substring);
          APPEND (msg, " of ");
          sprintf (substring, "%lu", info->md.file_size);
          APPEND (msg, substring);
          APPEND (msg, " bytes sent.\n");
        }
      else
        APPEND (msg, "(no file transfer)\n");
    }

  else
    {
      if (!info->has_md_been_received)
        APPEND (msg, "(waiting for MD)\n");
      else
        {
          if (info->md.file_transfer)
            {
              sprintf (substring, "%lu", info->fd_offset);
              APPEND (msg, substring);
              APPEND (msg, " of ");
              sprintf (substring, "%lu", info->md.file_size);
              APPEND (msg, substring);
              APPEND (msg, " bytes sent.\n");
            }
          else
            APPEND (msg, "(no file transfer)\n");
        }
    }

  /*---------------------------*/
  /* Finally, send the message */
  /*---------------------------*/
  i_msg__ (msg);
}



/*=r=************************************************************************/
static void m__generic_indication (INDICATION_TYPE type, 
                                   const TRANS_STATUS *info)
{
  char             msg [1024];
  char             substring [256];
  /*------------------------------------------------------------*/

  /* Build a simple message */
  strcpy (msg,":::");
  APPEND (msg, cfdp_indication_type_as_string (type));
  APPEND (msg, "  ");
  sprintf (substring, "trans %s.\n", cfdp_trans_as_string (info->trans));
  APPEND (msg, substring);

  /* Output the message */
  i_msg__ (msg);
}



/*=r=************************************************************************/
void indication__ (INDICATION_TYPE type, const TRANS_STATUS *info)
{
  /*------------------------------------------------------------*/

  /* If enabled, output a simple message */
  if (cfdp_is_message_class_enabled (CFDP_MSG_INDICATIONS))
    {
      if (type == IND_ABANDONED)
        m__abandoned (info);
      else if (type == IND_FAULT)
        m__fault (info);
      else if (type == IND_FILE_SEGMENT_RECV)
        m__file_segment_recv (info);
      else if (type == IND_FILE_SEGMENT_SENT)
        m__file_segment_sent (info);
      else if (type == IND_METADATA_RECV)
        m__metadata_recv (info);
      else if (type == IND_METADATA_SENT)
        m__metadata_sent (info);
      else if (type == IND_REPORT)
        m__report (info);
      else if (type == IND_TRANSACTION_FINISHED)
        m__transaction_finished (info);
      else if (type == IND_TRANSACTION)
        m__transaction (info);
      else
        m__generic_indication (type, info);
    }

  /* If the library user has registered a callback for Indications, call it */
  if (indication_callback != NULL)
    indication_callback (type, *info);
}
