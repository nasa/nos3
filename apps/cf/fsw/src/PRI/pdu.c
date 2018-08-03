/* FILE: pdu.c
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
 * SPECS:  See pdu.h
 * ORIGINAL PROGRAMMER:  Tim Ray 301-286-0581
 * CHANGES:
 *   2007_07_31 Tim Ray (TR)
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 *   2007_11_09 TR
 *     - Better portability.  C++ uses 'new' as a keyword, so any use of
 *       'new' as a variable name was changed to 'neww'.
 *   2007_12_03 TR
 *     - Better screening of incoming pdus.  
 *       a) the stated pdu-length must be big enough to hold the header plus
 *          the data_field.
 *       b) the pdu must be addressed to our entity-id.
 *   2008_01_02 TR
 *     - Removed unneeded assertions (validations are now performed within
 *       'pdu__is_this_pdu_acceptable').
 *   2008_03_03 TR
 *     - Safer code.  Added parentheses around the value of each macro that
 *       manipulates data 
 *       e.g. 'BYTE_4_OF_4' is now '( v % 256 )' instead of 'v % 256'
 *     - Bug fix.  In 'pdu__nak_struct_from_nak_pdu', don't try to read
 *       another gap unless there is enough remaining input to contain 
 *       a gap.   i.e. 8 more bytes.  (1 more byte is not enough)
 *       (the old code assumed the input would always be a multiple of
 *       8 bytes in length; the new code does not).
 *     - Change one message from 'info' to 'warning' (having to do with
 *       'Incoming Nak truncated').
 *     - Previous to this, the segmentation-field within incoming 
 *       Metadata PDUs was ignored.  It is now copied into the MD structure.
 */

#include "cfdp.h"
#include "cfdp_private.h"

#define  BYTE_1_OF_4(v)  ( v / (256*256*256) )
#define  BYTE_2_OF_4(v)  ( (v / (256*256)) % 256 )
#define  BYTE_3_OF_4(v)  ( (v / 256) % 256 )
#define  BYTE_4_OF_4(v)  ( v % 256 )
#define  LOAD_LOWER_NIBBLE(b,n)  ( (b | n) )
#define  LOAD_UPPER_NIBBLE(b,n)  ( (b | (n << 4)) )
#define  LOWER_NIBBLE(b) ( (b & 0x0F) )
#define  UPPER_NIBBLE(b) ( ((b & 0xF0) >> 4) )

#define  ASCII_TERMINATOR  0
#define  MIN_VALID_PDU_HEADER_LENGTH  6
#define  NO  0
#define  STRING_TERMINATOR  '\0'
#define  YES  1

typedef struct
{
  u_int_1      length;
  u_int_1      value [MAX_FILE_NAME_LENGTH];
} LV;

typedef struct
{
  int            start_of_source_id;
  u_int_1        length_of_source_id;
  int            start_of_trans_seq_num;
  u_int_1        length_of_trans_seq_num;
  int            start_of_dest_id;
  u_int_1        length_of_dest_id;
  int            length_of_header;
} PDU_LAYOUT;




/*=r=************************************************************************/
static PDU_LAYOUT m__pdu_layout (const DATA *pdu)
     /* WHAT IT DOES:  Determines (and returns) the layout of the given 
      *   raw PDU.  
      * NOTE:  This routine wouldn't exist if the PDU-header had a fixed
      *   format, but it doesn't (some fields in the PDU-header are 
      *   variable-length).
      */
   {
     PDU_LAYOUT    layout;
   /*------------------------------------------------------------*/

     layout.start_of_source_id = 4;
     layout.length_of_source_id = ((pdu->content[3] & 0x70) >> 4) + 1;
     layout.length_of_trans_seq_num = (pdu->content[3] & 0x07) + 1;
     layout.length_of_dest_id = layout.length_of_source_id;

     layout.start_of_trans_seq_num = layout.start_of_source_id + 
       layout.length_of_source_id;
     layout.start_of_dest_id = layout.start_of_trans_seq_num + 
       layout.length_of_trans_seq_num;
     layout.length_of_header = layout.start_of_dest_id + 
       layout.length_of_dest_id;

     return (layout);
   }



/*=r=************************************************************************/
static void m__append_4_byte_integer (PDU *pdu, u_int_4 integer)
     /* WHAT IT DOES:  Appends the given 4-byte integer to the end of the
      *   given PDU.
      */
   {
   /*------------------------------------------------------------*/
     pdu->content[pdu->length] = BYTE_1_OF_4 (integer);
     pdu->length ++;
     pdu->content[pdu->length] = BYTE_2_OF_4 (integer);
     pdu->length ++;
     pdu->content[pdu->length] = BYTE_3_OF_4 (integer);
     pdu->length ++;
     pdu->content[pdu->length] = BYTE_4_OF_4 (integer);
     pdu->length ++;
   }



/*=r=************************************************************************/
static void m__append_n_bytes (PDU *pdu, void *bytes, int n)
     /* WHAT IT DOES:  Appends the given n-byte long 'bytes' to the given 
      *   Pdu.  
      * INPUTS:
      *   'pdu' contains a Pdu.
      *   'bytes' points to the location of data to be appended.
      *   'n' tells how many bytes are to be appended.
      */
   {
   /*------------------------------------------------------------*/
     memcpy (&pdu->content[pdu->length], bytes, n);
     pdu->length += n;
   }



/*=r=************************************************************************/
static u_int_2 m__data_field_length (const DATA *pdu)
     /* WHAT IT DOES:  Given a raw PDU, it returns the length of that PDU's
      *   data field.
      */
   {
     u_int_2         data_field_length;
     u_int_1         lsb;
     u_int_1         msb;
   /*------------------------------------------------------------*/

     msb = pdu->content[1];
     lsb = pdu->content[2];
     data_field_length = (msb * 256) + lsb;

     return (data_field_length);
   }



/*=r=************************************************************************/
static ID m__dest_id (const DATA *pdu)
     /* WHAT IT DOES:  Given a raw PDU, it returns the Destination-ID from
      *   within the PDU header.
      */
   {
     ID                dest_id;
     int               i;
     PDU_LAYOUT        layout;
   /*------------------------------------------------------------*/

     /* Determine the location and length of the Dest-ID field in this pdu */
     layout = m__pdu_layout (pdu);

     /* Copy its contents */
     dest_id.length = layout.length_of_dest_id;
     for (i=0; i<dest_id.length; i++)
       dest_id.value[i] = pdu->content[i+layout.start_of_dest_id];
     
     return (dest_id);
   }



/*=r=************************************************************************/
static u_int_4 m__extract_4_byte_int (const u_int_1 *data)
     /* WHAT IT DOES:  Given 4 bytes of data, it returns a 4-byte unsigned
      *   integer value.
      * ASSUMPTION:  The input data goes from MSB to LSB.
      */
   {
     u_int_4           answer;
   /*------------------------------------------------------------*/
     answer = data[0];
     answer = (answer * 256) + data[1];
     answer = (answer * 256) + data[2];
     answer = (answer * 256) + data[3];

     return (answer);
   }



/*=r=************************************************************************/
static char *m__file_dir_code_as_string (FILE_DIR_CODE code)
     /* WHAT IT DOES:  Converts the given File Directive code to an ascii
      *   string representation.
      */
#define MAX_STRING_LENGTH 32
   {
     static char      cfdp_file_dir_as_string [][16] =
     {
       "RESERVED_0", "RESERVED_1", "RESERVED_2", "RESERVED_3", 
       "EOF", "FIN", "ACK", "MD",
       "NAK", "PROMPT", "SUSPEND", "RESUME", 
       "KEEPALIVE", "RESERVED_13", "RESERVED_14", "RESERVED_15" 
     };
     static char      string [MAX_STRING_LENGTH+1];
   /*------------------------------------------------------------*/

     if (code <= CFDP_FILE_DIR_RESERVED_15)
       COPY (string, cfdp_file_dir_as_string[code]);
     else
       sprintf (string, "Unknown (value=%2.2xx)", code);
     return (string);
   }
#undef MAX_STRING_LENGTH



/*=r=************************************************************************/
static void m__gen_pdu_header (HDR hdr, u_int_2 data_field_length, 
                               DATA *raw_header)
     /* WHAT IT DOES:  Given the PDU-header expressed as a C structure, 
      *   and given the length of the data-field, this routine returns
      *   a raw PDU-header.
      */
   {
     u_int_1           byte;
     u_int_1           entity_id_length;
     int               i;
     u_int_1           lower_byte;
     u_int_1           upper_byte;

   /*------------------------------------------------------------*/
     /* Initialize the raw Header */
     raw_header->length = 0;

     /*----------------------*/
     /* Byte 1 of the header */
     /*----------------------*/
     byte = 0;
     if (hdr.pdu_type == FILE_DATA_PDU)
       byte = byte | 0x10;
     if (hdr.direction == TOWARD_SENDER)
       byte = byte | 0x08;
     if (hdr.mode == UNACK_MODE)
       byte = byte | 0x04;
     m__append_n_bytes (raw_header, &byte, 1);

     /*----------------------------------------------------*/
     /* Generate and append the 2-byte "Data field length" */
     /*----------------------------------------------------*/
     upper_byte = data_field_length / 256;
     m__append_n_bytes (raw_header, &upper_byte, 1);
     lower_byte = data_field_length % 256;
     m__append_n_bytes (raw_header, &lower_byte, 1);
     
     /* <NOT_SUPPORTED> CRC in PDUs */

     /*-------------------------------------------------------------------*/
     /* Generate and append the 1-byte Entity-id length & trans-id length */
     /*-------------------------------------------------------------------*/

     /* First, determine the appropriate length for entity-IDs */
     if (hdr.dest_id.length >= hdr.trans.source_id.length)
       entity_id_length = hdr.dest_id.length;
     else 
       entity_id_length = hdr.trans.source_id.length;

     byte = 0;
     byte = byte | ((entity_id_length - 1) << 4);
     byte = byte | (HARD_CODED_TRANS_SEQ_NUM_LENGTH - 1);
     m__append_n_bytes (raw_header, &byte, 1);

     /*------------------------------------------------*/
     /* Append the var-length "source entity-id" field */
     /*------------------------------------------------*/

     /* Because both ID lengths (source & destination) have to be the same,
      * it may be necessary to add zeros (they are added on the left)
      */
     byte = 0;
     for (i=hdr.trans.source_id.length; i<hdr.dest_id.length; i++)
       m__append_n_bytes (raw_header, &byte, 1);
     m__append_n_bytes (raw_header, &hdr.trans.source_id.value, 
                        hdr.trans.source_id.length);

     /*-----------------------------------------------------*/
     /* Append the var-length "trans sequence number" field */
     /*-----------------------------------------------------*/

     /* The trans-sequence-number field length (for outgoing PDUs) is
      * hard-coded within this implementation of CFDP.  
      * (See HARD_CODED_TRANS_SEQ_NUM_LENGTH). 
      */
     m__append_4_byte_integer (raw_header, hdr.trans.number);

     /*-----------------------------------------------------*/
     /* Append the var-length "destination entity-id" field */
     /*-----------------------------------------------------*/

     /* Because both ID lengths (source & destination) have to be the same,
      * it may be necessary to add zeros (they are added on the left)
      */
     byte = 0;
     for (i=hdr.dest_id.length; i<hdr.trans.source_id.length; i++)
       m__append_n_bytes (raw_header, &byte, 1);
     m__append_n_bytes (raw_header, &hdr.dest_id.value, hdr.dest_id.length);
   }



/*=r=************************************************************************/
static PDU_MODE m__mode (const DATA *pdu)
     /* WHAT IT DOES:  Returns the mode of the given raw PDU. */
   {
   /*------------------------------------------------------------*/

     if (pdu->content[0] & 0x04)
       return (UNACK_MODE);
     else
       return (ACK_MODE);
   }



/*=r=************************************************************************/
static TRANSACTION m__transaction (const DATA *pdu)
     /* WHAT IT DOES:  Returns the transaction info from the given raw PDU. */
   {
     int               i;
     PDU_LAYOUT        layout;
     TRANSACTION       trans;
     int               trans_seq_num_length;
   /*------------------------------------------------------------*/
     /* Determine where the Source-ID field is located in this pdu */
     layout = m__pdu_layout (pdu);

     /* Copy its contents */
     trans.source_id.length = layout.length_of_source_id;
     for (i=0; i<trans.source_id.length; i++)
       trans.source_id.value[i] = pdu->content[i+layout.start_of_source_id];

     trans_seq_num_length = layout.length_of_trans_seq_num;
     if (trans_seq_num_length > HARD_CODED_TRANS_SEQ_NUM_LENGTH)
       /* Uh-oh, our assumption has proven wrong.  We assumed that no 
        * CFDP partner would use a transaction-sequence-number field
        * longer than 4 bytes.  
        */
       {
         e_msg__ ("cfdp_engine: Truncating trans-seq-num "
                  "(%u bytes -> %u bytes).\n",
                  trans_seq_num_length, HARD_CODED_TRANS_SEQ_NUM_LENGTH);
         /* Do the best we can (use least significant 4 bytes) */
         while (trans_seq_num_length > HARD_CODED_TRANS_SEQ_NUM_LENGTH)
           {
             layout.start_of_trans_seq_num ++;
             trans_seq_num_length --;
           }
       }
     trans.number = 0;
     for (i=0; i<trans_seq_num_length; i++)
       trans.number = (trans.number * 256) + 
         pdu->content[i+layout.start_of_trans_seq_num];

     return (trans);
   }



#define MINIMUM_VALID_PDU_LENGTH 9
/*=r=*************************************************************************/
boolean pdu__is_this_pdu_acceptable (PDU *pdu)
{
  int           data_field_length;
  int           dir_code;
  int           entity_id_length;
  int           i;
  int           length_of_pdu_header;
  ID            my_id;
  ID            pdu_id;
  int           pdu_type;
  int           start_of_dest_id;
  int           trans_id_length;
   /*------------------------------------------------------------*/

  /* NOTE:  Since the point of this routine is to screen out unacceptable
   *   pdus before they are passed to the "real" pdu conversion routines,
   *   this routine is self-contained (ie. it doesn't use any of the
   *   utility routines).   Also, efficiency is given priority over
   *   "easy to understand".
   */

  /*A valid pdu must be at least 'MINIMUM_VALID_PDU_LENGTH' bytes long */
  if (pdu->length < MINIMUM_VALID_PDU_LENGTH)
    {
      e_msg__ ("cfdp_engine: PDU rejected due to length "
               "(%lu bytes); must be >= %lu.\n",
               pdu->length, MINIMUM_VALID_PDU_LENGTH);
      return (0);
    }
  
  /* (Pull out some information from the pdu-header) */
  data_field_length = (pdu->content[1] * 256) + pdu->content[2];
  entity_id_length = ((pdu->content[3] & 0x70) >> 4) + 1;
  trans_id_length = (pdu->content[3] & 0x07) + 1;
  length_of_pdu_header = 4 + (2 * entity_id_length) + trans_id_length;

  /* A valid pdu must be as long as the "header + data_field" */
  if (pdu->length < (length_of_pdu_header + data_field_length))
    {
      e_msg__ ("cfdp_engine: PDU rejected due to length "
               "(%lu bytes); must be >= %lu.\n",
               pdu->length, length_of_pdu_header + data_field_length);
      return (0);
    }

  /* A valid pdu must have some data following the header */
  if (data_field_length == 0)
    {
      e_msg__ ("cfdp_engine: PDU rejected due to data-field-length (0); "
               "must be > 0.\n");
      return (0);
    }

  /* A valid File Directive PDU must specify a valid File Directive Code */
  pdu_type = pdu->content[0] && 0x10;
  if (pdu_type == 0x00)
    /* This is a File Directive PDU */
    {
      /* First byte after pdu-header is the File Directive Code */
      dir_code = pdu->content[length_of_pdu_header];
      /* See Table 5-4 in the CFDP Blue Book for dir-code values */
      if ((dir_code < 4) || (dir_code > 0x0C))
        {
          e_msg__ ("cfdp_engine: PDU rejected due to unrecognized "
                   "Directive Code (%2.2x).\n",
                   dir_code);
          return (NO);
        }
    }

  /* Our 'PDU_AS_STRUCT' structure can't hold an entity-id whose length is
   * greater than 'MAX_ID_LENGTH'
   */
  if (entity_id_length > MAX_ID_LENGTH)
    {
      e_msg__ ("cfdp_engine: PDU rejected due to entity-id length "
               "(%u); MAX_ID_LENGTH=%u.\n",
               entity_id_length, MAX_ID_LENGTH);
      return (NO);
    }

  /* Our 'PDU_AS_STRUCT' structure can't hold a trans-seq-num whose length
   * is greater than 4.
   */
  if (trans_id_length > 4)
    {
      e_msg__ ("cfdp_engine: PDU rejected due to trans length "
               "(%u); max supported is 4.\n",
               trans_id_length);
      return (NO);
    }

  /*--------------------------------------------*/
  /* The PDU must be addressed to our entity-id */
  /*--------------------------------------------*/

  /* Get our entity-id */
  my_id = mib__get_my_id ();

  /* Extract the (appropriate) entity-id from the pdu-header */
  if (pdu->content[0] & 0x08)
    /* The direction of this pdu is "towards the file sender" --
     * i.e. we are the source of the transaction.
     * So extract the source-id from the pdu.
     */
    {
      pdu_id.length = entity_id_length;
      /* (The source-id always starts at byte 4 of the header) */
      for (i=0; i<entity_id_length; i++)
        pdu_id.value[i] = pdu->content[i+4];
    }
  else
    /* The direction of this pdu is "towards the file receiver" --
     * i.e. we are the destination of the transaction.
     * So extract the dest-id from the pdu.
     */
    {
      pdu_id.length = entity_id_length;
      /* (The dest-id is after the source-id and trans-seq-number) */
      start_of_dest_id = 4 + entity_id_length + trans_id_length;
      for (i=0; i<entity_id_length; i++)
        pdu_id.value[i] = pdu->content[i+start_of_dest_id];
    }

  /* The 2 entity-ids should match */
  if (!cfdp_are_these_ids_equal (my_id, pdu_id))
    {
      e_msg__ ("cfdp_engine: PDU rejected due to invalid dest id (%s).\n",
               cfdp_id_as_string (pdu_id));
      return (NO);
    }

  return (YES);
}



/*=r=************************************************************************/
void pdu__make_md_pdu (HDR hdr, MD md, PDU *pdu)
   {
     u_int_1           byte;
     u_int_2           data_field_length;
     LV                lv;
     u_int_4           size;
   /*------------------------------------------------------------*/

     /*-------------------------*/
     /* Generate the PDU-header */
     /*-------------------------*/

     /* Metadata pdu is always a file-directive */
     hdr.pdu_type = FILE_DIR_PDU;

     /* Determine data-field length (see page 5-8 of spec) */
     data_field_length = 6;
     if (md.file_transfer)
       {
         data_field_length += strlen (md.source_file_name) + 1;
         data_field_length += strlen (md.dest_file_name) + 1;
       }
     else
       data_field_length += 2;

     /* <NOT_SUPPORTED> Metadata TLVs (the length calculation above
      * will need to change).
      */

     m__gen_pdu_header (hdr, data_field_length, pdu);

     /*------------------------------------*/
     /* Generate and append the data-field */
     /*------------------------------------*/

     /* File-dir code */
     byte = MD_PDU;
     m__append_n_bytes (pdu, &byte, 1);

     /* Byte 1 of MD */
     byte = 0;
     if (!md.segmentation_control)
       byte = byte | 0x80;
     m__append_n_bytes (pdu, &byte, 1);

     /* Bytes 2-5 (file size) */
     size = md.file_size;
     m__append_4_byte_integer (pdu, size);

     /* Source file name in LV format */
     lv.length = strlen (md.source_file_name);
     memcpy (lv.value, md.source_file_name, lv.length);
     m__append_n_bytes (pdu, &lv, lv.length+1);

     /* Dest file name in LV format */
     lv.length = strlen (md.dest_file_name);
     memcpy (lv.value, md.dest_file_name, lv.length);
     m__append_n_bytes (pdu, &lv, lv.length+1);

     /* <NOT_SUPPORTED> TLVs in Metadata PDU */
   }



/*=r=************************************************************************/
void pdu__make_fd_pdu (HDR hdr, FD *fd, PDU *pdu)
   {
     u_int_2           data_field_length;
   /*------------------------------------------------------------*/

     /*-------------------------*/
     /* Generate the PDU-header */
     /*-------------------------*/

     /* The pdu-type is file-data rather than file-directive */
     hdr.pdu_type = FILE_DATA_PDU;

     /* Determine data-field length (see page 5-11 of spec) */
     data_field_length = 4 + fd->buffer_length;

     m__gen_pdu_header (hdr, data_field_length, pdu);

     /*------------------------------------*/
     /* Generate and append the data-field */
     /*------------------------------------*/

     /* Offset */
     m__append_4_byte_integer (pdu, fd->offset);

     /* File-data */
     m__append_n_bytes (pdu, fd->buffer, fd->buffer_length);
   }



/*=r=************************************************************************/
void pdu__make_eof_pdu (HDR hdr, EOHF eof, PDU *pdu)
   {
     u_int_1           byte;
     u_int_2           data_field_length;
   /*------------------------------------------------------------*/

     /*-------------------------*/
     /* Generate the PDU-header */
     /*-------------------------*/

     /* Eof pdu is always a file-directive */
     hdr.pdu_type = FILE_DIR_PDU;

     /* Determine data-field length (see page 5-xx of spec) */
     data_field_length = 10;
     m__gen_pdu_header (hdr, data_field_length, pdu);

     /*------------------------------------*/
     /* Generate and append the data-field */
     /*------------------------------------*/

     /* File-directive code */
     byte = EOF_PDU;
     m__append_n_bytes (pdu, &byte, 1);

     /* Byte 1 of EOF-data */
     byte = 0;
     byte = (byte | eof.condition_code) << 4;
     byte = byte | eof.spare;
     m__append_n_bytes (pdu, &byte, 1);

     /* File checksum */
     m__append_4_byte_integer (pdu, eof.file_checksum);

     /* File size */
     m__append_4_byte_integer (pdu, eof.file_size);
   }


/*=r=************************************************************************/
void pdu__make_fin_pdu (HDR hdr, FIN fin, PDU *pdu)
   {
     u_int_1           byte;
     u_int_2           data_field_length;
   /*------------------------------------------------------------*/

     /*-------------------------*/
     /* Generate the PDU-header */
     /*-------------------------*/

     /* Fin pdu is always a file-directive */
     hdr.pdu_type = FILE_DIR_PDU;

     /* Determine data-field length (see page 5-xx of spec) */
     data_field_length = 2;   /* <NOT_SUPPORTED> TLVs 
                                 (will affect length calculation) */
     m__gen_pdu_header (hdr, data_field_length, pdu);

     /*------------------------------------*/
     /* Generate and append the data-field */
     /*------------------------------------*/

     /* File-directive code */
     byte = FIN_PDU;
     m__append_n_bytes (pdu, &byte, 1);

     /* Byte 1 of FIN-data */
     byte = 0;
     byte = byte | (fin.condition_code << 4);
     byte = byte | (fin.end_system_status << 3);
     byte = byte | (fin.delivery_code << 2);
     byte = byte | fin.spare;
     m__append_n_bytes (pdu, &byte, 1);
     
     /* <NOT_SUPPORTED> Filestore Requests 
      * (Filestore Response(s) would be added here)
      */
     ;
   }


/*=r=************************************************************************/
void pdu__make_ack_pdu (HDR hdr, ACK ack, PDU *pdu)
   {
     u_int_1           byte;
     u_int_2           data_field_length;
   /*------------------------------------------------------------*/

     /*-------------------------*/
     /* Generate the PDU-header */
     /*-------------------------*/

     /* Ack pdu is always a file-directive */
     hdr.pdu_type = FILE_DIR_PDU;

     /* Determine data-field length (see page 5-xx of spec) */
     data_field_length = 3;
     m__gen_pdu_header (hdr, data_field_length, pdu);

     /*------------------------------------*/
     /* Generate and append the data-field */
     /*------------------------------------*/

     /* File-directive code */
     byte = ACK_PDU;
     m__append_n_bytes (pdu, &byte, 1);

     /* Byte 1 of ACK data */
     byte = 0;
     byte = byte | (ack.directive_code << 4);
     byte = byte | ack.directive_subtype_code;
     m__append_n_bytes (pdu, &byte, 1);

     /* Byte 2 of ACK data */
     byte = 0;
     byte = byte | (ack.condition_code << 4);
     byte = byte | (ack.delivery_code << 2);
     byte = byte | ack.transaction_status;
     m__append_n_bytes (pdu, &byte, 1);
   }



/*=r=************************************************************************/
void pdu__make_nak_pdu (HDR hdr, NAK nak, PDU *pdu)
   {
     u_int_1           byte;
     u_int_2           data_field_length;
     u_int_4           how_many_gaps;
     GAP              *p;
   /*------------------------------------------------------------*/

     /*-------------------------*/
     /* Generate the PDU-header */
     /*-------------------------*/

     /* Nak pdu is always a file-directive */
     hdr.pdu_type = FILE_DIR_PDU;

     /* Determine data-field length (see page 5-xx of spec) */
     how_many_gaps = nak__how_many_filedata_gaps(&nak);
     if (nak.is_metadata_missing)
       how_many_gaps ++;
     if (how_many_gaps >= MAX_GAPS_PER_NAK_PDU)
       /* There are more gaps than will fit in one Nak PDU */
       {
         d_msg__ ("cfdp_engine: outgoing Nak has first %u of %lu "
                  "total gaps\n",
                  MAX_GAPS_PER_NAK_PDU, how_many_gaps);
         how_many_gaps = MAX_GAPS_PER_NAK_PDU;
       }

     data_field_length = 9 + (8 * how_many_gaps);
     m__gen_pdu_header (hdr, data_field_length, pdu);

     /*------------------------------------*/
     /* Generate and append the data-field */
     /*------------------------------------*/

     /* File-directive code */  
     byte = NAK_PDU;
     m__append_n_bytes (pdu, &byte, 1);

     /* (Variable-length) list of gaps */
     m__append_4_byte_integer (pdu, nak.start_of_scope);
     m__append_4_byte_integer (pdu, nak.end_of_scope);
     if (nak.is_metadata_missing)
       /* Missing Metadata is represented in the raw PDU as a gap of 0-0 */
       {
         m__append_4_byte_integer (pdu, 0);
         m__append_4_byte_integer (pdu, 0);
         how_many_gaps --;
       }
         
     for (p=nak.head; p!=NULL; p=p->next)
       {
         if (how_many_gaps == 0)
           break;
         m__append_4_byte_integer (pdu, p->begin);
         m__append_4_byte_integer (pdu, p->end);
         how_many_gaps --;
       }
   }



/*oooooo*/
/*=r=************************************************************************/
_PDU_TYPE_ pdu__what_type (const PDU *pdu)
   {
     ACK               ack_struct;
     _PDU_TYPE_        type;
     FILE_DIR_CODE     dir_code;
   /*------------------------------------------------------------*/

     type = _DONT_KNOW_;
     if (pdu__type(pdu) == FILE_DATA_PDU)
       /* This is a File-Data PDU */
       type = _FD_;
     else 
       /* This is one of the File-Directive PDUs */
       {
         dir_code = pdu__file_dir_code (pdu);
         if (dir_code == MD_PDU)
           type = _MD_;
         else if (dir_code == EOF_PDU)
           type = _EOF_;
         else if (dir_code == NAK_PDU)
           type = _NAK_;
         else if (dir_code == FIN_PDU)
           type = _FIN_;
         else if (dir_code == ACK_PDU)
           {
             ack_struct = pdu__ack_struct_from_ack_pdu (pdu);
             if (ack_struct.directive_code == EOF_PDU)
               type = _ACK_EOF_;
             else if (ack_struct.directive_code == FIN_PDU)
               type = _ACK_FIN_;
             else
               /* This Ack should have been for either an EOF or Fin PDU */
               w_msg__ ("cfdp_engine: Ack is of neither EOF nor Finished.\n");
           }
       }

     return (type);
   }



/*=r=************************************************************************/
PDU_TYPE pdu__type (const DATA *pdu)
   {
   /*------------------------------------------------------------*/

     if (pdu->content[0] & 0x10)
       return (FILE_DATA_PDU);
     else
       return (FILE_DIR_PDU);
   }



/*=r=************************************************************************/
void pdu__convert_pdu_to_struct (const PDU *pdu, PDU_AS_STRUCT *p_a_s)
   {
   /*------------------------------------------------------------*/

     /* Convert the raw PDU Header to a HDR structure */
     p_a_s->hdr = pdu__hdr_struct_from_pdu (pdu);

     /* Convert the rest of the PDU; it's either File-Data or a 
      * File-Directive.
      */

     if (pdu__type(pdu) == FILE_DATA_PDU)
       /* Convert the File-Data portion of the PDU to a FD structure */
       {
         p_a_s->is_this_a_file_data_pdu = YES;
         pdu__fd_struct_from_fd_pdu (pdu, &(p_a_s->data_field.fd));
       }

     else
       /* Convert the File-Directive portion of the PDU to the appropriate
        * structure.
        */
       {
         p_a_s->is_this_a_file_data_pdu = NO;
         p_a_s->dir_code = pdu__file_dir_code (pdu);
         if (p_a_s->dir_code == MD_PDU)
           p_a_s->data_field.md = pdu__md_struct_from_md_pdu (pdu);
         else if (p_a_s->dir_code == EOF_PDU)
           p_a_s->data_field.eof = pdu__eof_struct_from_eof_pdu (pdu);
         else if (p_a_s->dir_code == FIN_PDU)
           p_a_s->data_field.fin = pdu__fin_struct_from_fin_pdu (pdu);
         else if (p_a_s->dir_code == ACK_PDU)
           p_a_s->data_field.ack = pdu__ack_struct_from_ack_pdu (pdu);
         else if (p_a_s->dir_code == NAK_PDU)
           pdu__nak_struct_from_nak_pdu (pdu, &p_a_s->data_field.nak);
         else
           e_msg__ ( "cfdp_engine: Unrecognized File-Dir in incoming pdu\n");
       }
   }



/*=r=************************************************************************/
HDR pdu__hdr_struct_from_pdu (const PDU *pdu)
   {
     HDR            hdr;
   /*------------------------------------------------------------*/

     hdr.pdu_type = pdu__type (pdu);
     if (pdu->content[0] & 0x08)
       hdr.direction = TOWARD_SENDER;
     else
       hdr.direction = TOWARD_RECEIVER;
     hdr.mode = m__mode (pdu);
     hdr.use_crc = NO;  /* <NOT_SUPPORTED> CRC in PDUs */
     hdr.trans = m__transaction (pdu);
     hdr.dest_id = m__dest_id (pdu);
     return (hdr);
   }



/*=r=************************************************************************/
FILE_DIR_CODE pdu__file_dir_code (const DATA *pdu)
   {
     FILE_DIR_CODE    file_dir_code;
     PDU_LAYOUT       layout;
     int              start_of_pdu_data_field;
   /*------------------------------------------------------------*/

     /* Determine where the pdu data-field is located in this pdu */
     layout = m__pdu_layout (pdu);
     start_of_pdu_data_field = layout.length_of_header;

     /* Copy the File-directive Code (first byte of pdu-data-field) */
     file_dir_code = pdu->content[start_of_pdu_data_field];

     return (file_dir_code);
   }



/*=r=************************************************************************/
MD pdu__md_struct_from_md_pdu (const PDU *pdu)
{
  u_int_4         file_size;
  int             index;
  int             length;
  MD              md;
  PDU_LAYOUT      pdu_layout;
  int             safe_string_length;
  /*------------------------------------------------------------*/

  /* Skip past the PDU-header */
  pdu_layout = m__pdu_layout (pdu);
  index = pdu_layout.length_of_header;

  /* Skip over the file-dir-code */
  index ++;

  md.segmentation_control = pdu->content[index] & 0x80;
  /* (skip over the Segmentation field) */
  index ++; 

  /* Calculate the file_size */
  file_size = m__extract_4_byte_int (&(pdu->content[index]));
  index += 4;
  md.file_size = file_size;

  /* Copy the source-file-name into a null-terminated string */
  length = pdu->content[index];
  index ++;
  if (length > MAX_FILE_NAME_LENGTH)
    {
      w_msg__ ("cfdp_engine: Incoming Metadata PDU exceeds storage capacity; "
               "truncated source-file-name.\n");
      safe_string_length = MAX_FILE_NAME_LENGTH;
    }
  else
    safe_string_length = length;
  memcpy (md.source_file_name, &(pdu->content[index]), safe_string_length);
  md.source_file_name[safe_string_length] = ASCII_TERMINATOR;
  index += length;

  /* Copy the dest-file-name into a null-terminated string */
  length = pdu->content[index];
  index ++;
  if (length > MAX_FILE_NAME_LENGTH)
    {
      w_msg__ ("cfdp_engine: Incoming Metadata PDU exceeds storage capacity; "
               "truncated dest-file-name.\n");
      safe_string_length = MAX_FILE_NAME_LENGTH;
    }
  else
    safe_string_length = length;
  memcpy (md.dest_file_name, &(pdu->content[index]), safe_string_length);
  md.dest_file_name[safe_string_length] = ASCII_TERMINATOR;
  index += length;

  /* Determine whether or not trans includes file transfer */
  if (length > 0)
    md.file_transfer = YES;
  else
    /* No file name given; therefore no file transfer requested */
    md.file_transfer = NO;

  /* <NOT_SUPPORTED> TLVs in Metadata
   * (if they were supported, they would be copied into MD structure here)
   */

  return (md);
}



/*=r=************************************************************************/
void pdu__fd_struct_from_fd_pdu (const PDU *pdu, FD *fd)
{
  int             index;
  u_int_4         offset;
  PDU_LAYOUT      pdu_layout;
  /*------------------------------------------------------------*/

  /* Skip past the PDU-header */
  pdu_layout = m__pdu_layout (pdu);
  index = pdu_layout.length_of_header;

  /* Store the offset */
  offset = m__extract_4_byte_int (&(pdu->content[index]));
  index += 4;
  fd->offset = offset;

  /* Copy the data */
  fd->buffer_length = m__data_field_length(pdu) - 4;
  if (fd->buffer_length > MAX_FILE_CHUNK_SIZE)
    /* Oops.  Our internal structure for storing Filedata is not big enough
     * to hold this Filedata PDU.
     */
    {
      w_msg__ ("cfdp_engine: Incoming Filedata truncated (%lu / %lu).\n",
               MAX_FILE_CHUNK_SIZE, fd->buffer_length);
      fd->buffer_length = MAX_FILE_CHUNK_SIZE;
    }
  memcpy (fd->buffer, &(pdu->content[index]), fd->buffer_length);
}



/*=r=************************************************************************/
EOHF pdu__eof_struct_from_eof_pdu (const PDU *pdu)
{
  u_int_4         checksum;
  u_int_4         file_size;
  int             index;
  EOHF            eof;
  PDU_LAYOUT      pdu_layout;
  /*------------------------------------------------------------*/

  /* Skip past the PDU-header */
  pdu_layout = m__pdu_layout (pdu);
  index = pdu_layout.length_of_header;

  /* Skip over the file-dir-code */
  index ++;

  /* Store info from next byte */
  eof.condition_code = UPPER_NIBBLE (pdu->content[index]);
  eof.spare = LOWER_NIBBLE (pdu->content[index]);
  index ++;

  /* Store the checksum */
  checksum = m__extract_4_byte_int (&(pdu->content[index]));
  index += 4;
  eof.file_checksum = checksum;

  /* Store the file-size */
  file_size = m__extract_4_byte_int (&(pdu->content[index]));
  index += 4;
  eof.file_size = file_size;

  return (eof);
}



/*=r=************************************************************************/
FIN pdu__fin_struct_from_fin_pdu (const PDU *pdu)
{
  int             index;
  FIN             fin;
  PDU_LAYOUT      pdu_layout;
  /*------------------------------------------------------------*/

  /* Skip past the PDU-header */
  pdu_layout = m__pdu_layout (pdu);
  index = pdu_layout.length_of_header;

  /* Skip over the file-dir-code */
  index ++;

  /* Store info from next byte */
  fin.condition_code = UPPER_NIBBLE (pdu->content[index]);
  fin.end_system_status = (pdu->content[index] & 0x08) >> 3;
  fin.delivery_code = (pdu->content[index] & 0x04) >> 2;
  fin.spare = pdu->content[index] & 0x03;
  
  return (fin);
}



/*=r=************************************************************************/
ACK pdu__ack_struct_from_ack_pdu (const PDU *pdu)
{
  ACK             ack;
  int             index;
  PDU_LAYOUT      pdu_layout;
  /*------------------------------------------------------------*/

  /* Skip past the PDU-header */
  pdu_layout = m__pdu_layout (pdu);
  index = pdu_layout.length_of_header;

  /* Skip over the file-dir-code */
  index ++;

  /* Store info from next byte */
  ack.directive_code = UPPER_NIBBLE (pdu->content[index]);
  ack.directive_subtype_code = LOWER_NIBBLE (pdu->content[index]);
  index ++;

  /* Store info from next byte */
  ack.condition_code = UPPER_NIBBLE (pdu->content[index]);
  ack.delivery_code = (pdu->content[index] & 0x0C) >> 2;
  ack.transaction_status = pdu->content[index] & 0x03;

  return (ack);
}



/*=r=************************************************************************/
void pdu__nak_struct_from_nak_pdu (const PDU *pdu, NAK *nak)
{
  u_int_4         begin;
  u_int_4         end;
  GAP            *end_of_list;
  int             index;
  GAP            *neww;
  PDU_LAYOUT      pdu_layout;
  /*------------------------------------------------------------*/

  /* Skip past the PDU-header */
  pdu_layout = m__pdu_layout (pdu);
  index = pdu_layout.length_of_header;

  /* Skip over the file-dir-code */
  index ++;

  /* Store 'start_of_scope' */
  nak->start_of_scope = m__extract_4_byte_int (&(pdu->content[index]));
  index += 4;

  /* Store 'end_of_scope' */
  nak->end_of_scope = m__extract_4_byte_int (&(pdu->content[index]));
  index += 4;

  /*-----------------------------------------------------------------------*/
  /* If the first gap represents missing Metadata, convert it to a boolean */
  /*-----------------------------------------------------------------------*/

  if (index + 8 <= pdu->length)
    {
      begin = m__extract_4_byte_int (&(pdu->content[index]));
      index += 4;
      end = m__extract_4_byte_int (&(pdu->content[index]));
      index += 4;
      if ((begin == 0) && (end == 0))
        /* There is missing Metadata */
        nak->is_metadata_missing = YES;
      else
        /* Not missing; rewind the index so that this gap is handled below */
        {
          nak->is_metadata_missing = NO;
          index -= 8;
        }
    }

  /*--------------------------------------*/
  /* Store the variable-length 'nak-list' */
  /*--------------------------------------*/

  nak->head = NULL;
  end_of_list = NULL;
  nak_mem__init_heap (nak);
  while (index + 8 <= pdu->length)
    /* Transfer each gap from the Nak-pdu to the Nak-struct */
    {
      /* Create a new node */
      neww = nak_mem__malloc (nak);
      if (neww == NULL)
        /* Oops.  Out of memory.  Return our current Nak. */
        {
          w_msg__ ("cfdp_engine: Incoming Nak truncated to fit in storage.\n");
          return;
        }

      /* Transfer the current gap to this node */
      neww->begin = m__extract_4_byte_int (&(pdu->content[index]));
      index += 4;
      neww->end = m__extract_4_byte_int (&(pdu->content[index]));
      index += 4;

      /* Attach it to the end of the list */
      if (nak->head == NULL)
        /* The list is empty, so this new node is the head (first) node */
        nak->head = neww;
      else
        end_of_list->next = neww;

      /* This new node is now the end of the list */
      end_of_list = neww;
      neww->next = NULL;
    }
}



/*=r=************************************************************************/
ROLE pdu__receiver_role_from_pdu_hdr (HDR hdr)
   {
     ROLE           role;
   /*------------------------------------------------------------*/

     if (hdr.direction == TOWARD_RECEIVER)
       {
         if (hdr.mode == UNACK_MODE)
           role = R_1;
         else
           role = R_2;
       }
     else if (hdr.direction == TOWARD_SENDER)
       {
         if (hdr.mode == UNACK_MODE)
           role = S_1;
         else
           role = S_2;
       }
     else
       /* This code should never be reached
        * (the header direction should always be one of the two choices).
        * If this code is reached, report the bug.
        * If the assertion routine allows the program to continue,
        * return a "sensible" value to the caller.
        */
       {
         ASSERT__BUG_DETECTED;
         role = ROLE_UNDEFINED;
       }

     return (role);
   }



/*=r=************************************************************************/
void pdu__display_raw_pdu (DATA pdu)
   {
#define  MAX_BYTES_DISPLAYED  32
     int             i;
     char            string [MAX_BYTES_DISPLAYED*3+100];
     char            substring [32];
   /*------------------------------------------------------------*/
     
     /* Is the PDU "too short"? */
     if (pdu.length < 5)
       {
         d_msg__ ("   Pdu (raw):  Length is less than 5! (too short)\n");
         return;
       }
     
     strcpy (string, "   Pdu (raw):  ");

     if (pdu.length > MAX_BYTES_DISPLAYED)
       /* Let the user know that only part of the PDU is displayed */
       {
         sprintf (substring, "(first %u bytes) ", MAX_BYTES_DISPLAYED);
         APPEND (string, substring);
       }

     /* Now, show the individual bytes from within the PDU */
     for (i=0; i<pdu.length; i++)
       {
         sprintf (substring, "%2.2x ", pdu.content[i]);
         APPEND (string, substring);
         if (i >= MAX_BYTES_DISPLAYED)
           break;
       }

     /* Finally, output the string */
     d_msg__ ("%s.\n", string);
   }



/*=r=************************************************************************/
void pdu__display_disassembled_pdu (DATA pdu)
   {
     PDU_AS_STRUCT         p_a_s;
     char                  string [128];
     char                  substring [64];
   /*------------------------------------------------------------*/

     strcpy (string, "   Pdu: ");

     pdu__convert_pdu_to_struct (&pdu, &p_a_s);
     if (pdu__type (&pdu) == FILE_DATA_PDU)
       /* It's a File-Data PDU, so show the Offset and Length */
       {
         APPEND (string, "FD, ");
         sprintf (substring, "offset=%lu, length=%u, ", 
                  p_a_s.data_field.fd.offset,
                  p_a_s.data_field.fd.buffer_length);
         APPEND (string, substring);
       }
     else
       /* It's a File-Directive PDU, so show the Directive Code */
       {
         APPEND (string, 
                 m__file_dir_code_as_string (pdu__file_dir_code (&pdu)));
         APPEND (string, " ");
       }

     /* Show the transaction-ID */
     APPEND (string, "trans ");
     APPEND (string, cfdp_trans_as_string (p_a_s.hdr.trans));
     APPEND (string, " ");

     /* Is it Class 1 (unacknowledged) or Class 2 (acknowledged) ? */
     if (m__mode(&pdu) == ACK_MODE)
       APPEND (string, "(acked, ");
     else
       APPEND (string, "(unack, ");

     /* Is the PDU going from Sender to Receiver, or Receiver to Sender? */
     if (pdu.content[0] & 0x08)
       APPEND (string, "S<-R, ");
     else
       APPEND (string, "S->R, ");
     
     /* Does this PDU include a CRC? */
     if (pdu.content[0] & 0x02)
       APPEND (string, "w/ crc).");
     else
       APPEND (string, "no crc).");

     /* Finally, output the string */
     d_msg__ ("%s\n", string);
   }
