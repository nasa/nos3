/* FILE: callbacks.h -- specs for a module that allows the library user
 *   to register callback routines.  This module also stores the current
 *   pointer to each callback routine.
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
 * ORIGINAL PROGRAMMER:  Tim Ray 301-286-0581
 * CHANGES:
 *   2007_07_31 Tim Ray
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 */

#ifndef H_CALLBACKS
#define H_CALLBACKS 1

/* The callback routine pointers are global (available to all modules) */
extern void (*indication_callback) (INDICATION_TYPE, TRANS_STATUS);
extern int (*printf_debug_callback) (const char *, ...);
extern int (*printf_info_callback) (const char *, ...);
extern int (*printf_warning_callback) (const char *, ...);
extern int (*printf_error_callback) (const char *, ...);
extern boolean (*g__pdu_output_open) (ID, ID);
extern boolean (*g__pdu_output_ready) (PDU_TYPE, TRANSACTION, ID);
extern void (*g__pdu_output_send) (TRANSACTION, ID, DATA *);
extern int (*fclose_callback) (CFDP_FILE *file);
extern int (*feof_callback) (CFDP_FILE *file);
extern CFDP_FILE *(*fopen_callback) (const char *filename, const char *mode);
extern size_t (*fread_callback) (void *buffer, size_t size, 
                          size_t count, CFDP_FILE *file);
extern int (*fseek_callback) (CFDP_FILE *file, long int offset, int whence);
extern size_t (*fwrite_callback) (const void *buff, size_t size, 
                           size_t count, CFDP_FILE *file);

/* To rename a file */
extern int (*rename_callback) (const char *old, const char *new);

/* To remove (delete) a file */
extern int (*remove_callback) (const char *name);

/* How big is this file? */
extern u_int_4 (*file_size_callback) (const char *name);

/* To choose the name of a temporary file */
extern char *(*temp_file_name_callback) (char *string);

/* Is this file segmented? */
extern boolean (*is_file_segmented_callback) (const char *name);



/* These macros allow the CFDP library source code to be unchanged */
#define msg__ printf_info_callback
#define d_msg__ printf_debug_callback
#define i_msg__ printf_info_callback
#define w_msg__ printf_warning_callback
#define e_msg__ printf_error_callback
#define pdu_output__open g__pdu_output_open
#define pdu_output__ready g__pdu_output_ready
#define pdu_output__send g__pdu_output_send

/* Defaults are declared for some of the callback routines */
u_int_4 file_size (const char *name);
boolean is_file_segmented (const char *filename);
char *temp_file_name (char *string);

#endif
