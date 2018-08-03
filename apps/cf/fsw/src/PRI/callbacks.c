/* FILE: callbacks.c  -- Routines for registering callbacks, as well as
 *   storage of the callback function pointers.
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
 * SPECS:  cfdp.h
 * ORIGINAL PROGRAMMER:  Tim Ray 301-286-0581
 * CHANGES:
 *   2007_07_31 Tim Ray
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 */

#include "cfdp.h"
#include "cfdp_private.h"


/*--------------------------*/
/* Global callback pointers */
/*--------------------------*/

/* PDU output callbacks */
boolean (*g__pdu_output_open) (ID, ID) = NULL;
boolean (*g__pdu_output_ready) (PDU_TYPE, TRANSACTION, ID) = NULL;
void (*g__pdu_output_send) (TRANSACTION, ID, DATA *) = NULL;

/* Indication callback  */
void (*indication_callback) (INDICATION_TYPE, TRANS_STATUS) = NULL;

/* Printf callbacks */
int (*printf_debug_callback) (const char *, ...) = printf;
int (*printf_info_callback) (const char *, ...) = printf;
int (*printf_warning_callback) (const char *, ...) = printf;
int (*printf_error_callback) (const char *, ...) = printf;

/* Virtual Filestore callbacks */

int (*fclose_callback) (CFDP_FILE *file) = fclose;
int (*feof_callback) (CFDP_FILE *file) = feof;
CFDP_FILE *(*fopen_callback) (const char *filename, const char *mode) = fopen;
size_t (*fread_callback) (void *buffer, size_t size, 
                          size_t count, CFDP_FILE *file) = fread;
int (*fseek_callback) (CFDP_FILE *file, long int offset, int whence) = fseek;
size_t (*fwrite_callback) (const void *buff, size_t size, 
                           size_t count, CFDP_FILE *file) = fwrite;

int (*rename_callback) (const char *old, const char *new) = rename;
int (*remove_callback) (const char *name) = remove;

char *(*temp_file_name_callback) (char *string) = temp_file_name;
u_int_4 (*file_size_callback) (const char *name) = file_size;
boolean (*is_file_segmented_callback) (const char *name) = is_file_segmented;



/*------------------------------------*/
/* Routines for registering callbacks */
/*------------------------------------*/


/* PDU output */


/*=r=************************************************************************/
void register_pdu_output_open (boolean (*function) (ID, ID))
{
  /*------------------------------------------------------------*/
  g__pdu_output_open = function;
}


/*=r=************************************************************************/
void register_pdu_output_ready (boolean (*function) 
                                (PDU_TYPE, TRANSACTION, ID partner_id))
{
  /*------------------------------------------------------------*/
  g__pdu_output_ready = function;
}


/*=r=************************************************************************/
void register_pdu_output_send (void (*function) 
                               (TRANSACTION trans, ID partner_id, DATA *pdu))
{
  /*------------------------------------------------------------*/
  g__pdu_output_send = function;
}


/* Indications */


/*=r=************************************************************************/
void register_indication (void (*function) 
                          (INDICATION_TYPE, TRANS_STATUS))
{
  /*------------------------------------------------------------*/
  indication_callback = function;
}


/* Printf */


/*=r=************************************************************************/
void register_printf (int (*function) (const char *, ...))
{
  /*------------------------------------------------------------*/
  printf_debug_callback = function;
  printf_info_callback = function;
  printf_warning_callback = function;
  printf_error_callback = function;
}


/*=r=************************************************************************/
void register_printf_debug (int (*function) (const char *, ...))
{
  /*------------------------------------------------------------*/
  printf_debug_callback = function;
}


/*=r=************************************************************************/
void register_printf_info (int (*function) (const char *, ...))
{
  /*------------------------------------------------------------*/
  printf_info_callback = function;
}


/*=r=************************************************************************/
void register_printf_warning (int (*function) (const char *, ...))
{
  /*------------------------------------------------------------*/
  printf_warning_callback = function;
}


/*=r=************************************************************************/
void register_printf_error (int (*function) (const char *, ...))
{
  /*------------------------------------------------------------*/
  printf_error_callback = function;
}


/* Virtual Filestore */


/*=r=************************************************************************/
void register_fclose (int (*function) (CFDP_FILE *file))
{
  /*------------------------------------------------------------*/
  fclose_callback = function;
}


/*=r=************************************************************************/
void register_feof (int (*function) (CFDP_FILE *file))
{
  /*------------------------------------------------------------*/
  feof_callback = function;
}


/*=r=************************************************************************/
void register_fopen (CFDP_FILE *(*function) (const char *name, 
                                             const char *mode))
{
  /*------------------------------------------------------------*/
  fopen_callback = function;
}


/*=r=************************************************************************/
void register_fread (size_t (*function) (void *buffer, size_t size,
                                         size_t count, CFDP_FILE *file))
{
  /*------------------------------------------------------------*/
  fread_callback = function;
}


/*=r=************************************************************************/
void register_fseek (int (*function) (CFDP_FILE *file, long int offset,
                                      int whence))
{
  /*------------------------------------------------------------*/
  fseek_callback = function;
}


/*=r=************************************************************************/
void register_fwrite (size_t (*function) (const void *buff, size_t size,
                                          size_t count, CFDP_FILE *file))
{
  /*------------------------------------------------------------*/
  fwrite_callback = function;
}



/*=r=************************************************************************/
void register_rename (int (*function) (const char *old, const char *new))
{
  /*------------------------------------------------------------*/
  rename_callback = function;
}


/*=r=************************************************************************/
void register_remove (int (*function) (const char *name))
{
  /*------------------------------------------------------------*/
  remove_callback = function;
}


/*=r=************************************************************************/
void register_tmpnam (char *(*function) (char *string))
{
  /*------------------------------------------------------------*/
  temp_file_name_callback = function;
}



/*=r=************************************************************************/
void register_file_size (u_int_4 (*function) (const char *file_name))
{
  /*------------------------------------------------------------*/
  file_size_callback = function;
}


/*=r=************************************************************************/
void register_is_file_segmented (boolean (*function) (const char *file_name))
{
  /*------------------------------------------------------------*/
  is_file_segmented_callback = function;
}
