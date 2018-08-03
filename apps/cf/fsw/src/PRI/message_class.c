/* FILE: message_class.c -- a module that maintains message filter settings
 *   (i.e. "which classes of messages are currently enabled for output?").
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
 * SPECS:  see message_class.h
 * CHANGES:
 *   2007_12_05 Tim Ray (TR)
 *     - Initial release with version 3.0d
 */

#include "cfdp.h"
#include "cfdp_private.h"

#define NO 0
#define YES 1

static boolean          m_is_class_enabled [NUMBER_OF_MESSAGE_CLASSES];
static boolean          m_initialized = NO;





/*=r=*************************************************************************/
static void m__initialize (void)
     /* WHAT IT DOES:  Initializes the message filter settings. */
{
  int                     i;
  /*------------------------------------------------------------*/

  /* First, disable all message-classes */
  for (i=1; i<NUMBER_OF_MESSAGE_CLASSES; i++)
    m_is_class_enabled[i] = NO;

  /* If the engine user indicated that a message-class should be
   * enabled at compile-time (see PUB/cfdp_config.h), then do so.
   */

#if MSG_INDICATIONS==1
  m_is_class_enabled[CFDP_MSG_INDICATIONS] = YES;
#endif

#if MSG_DEBUG_MEMORY_USE==1
  m_is_class_enabled[CFDP_MSG_DEBUG_MEMORY_USE] = YES;
#endif

#if MSG_DEBUG_NAK==1
  m_is_class_enabled[CFDP_MSG_DEBUG_NAK] = YES;
#endif

#if MSG_DEBUG_PDU==1
  m_is_class_enabled[CFDP_MSG_DEBUG_PDU] = YES;
#endif

#if MSG_PDU_FILEDATA==1
  m_is_class_enabled[CFDP_MSG_PDU_FILEDATA] = YES;
#endif

#if MSG_PDU_NON_FILEDATA==1
  m_is_class_enabled[CFDP_MSG_PDU_NON_FILEDATA] = YES;
#endif

#if MSG_PDU_RETRANSMITTED_FD==1
  m_is_class_enabled[CFDP_MSG_PDU_RETRANSMITTED_FD] = YES;
#endif

#if MSG_STATE_ALL==1
  m_is_class_enabled[CFDP_MSG_STATE_ALL] = YES;
#endif

#if MSG_STATE_CHANGE==1
  m_is_class_enabled[CFDP_MSG_STATE_CHANGE] = YES;
#endif

  /* Make sure initialization happens only once */
  m_initialized = YES;
}



/*=r=*************************************************************************/
boolean message_class__enabled (int class)
{
  /*------------------------------------------------------------*/

  if (!m_initialized)
    m__initialize ();

  if (class >= NUMBER_OF_MESSAGE_CLASSES)
    return (0);

  return (m_is_class_enabled[class]);
}



/*=r=*************************************************************************/
boolean message_class__enable (int class)
{
  int                     i;
  /*------------------------------------------------------------*/

  if (!m_initialized)
    m__initialize ();

  if (class >= NUMBER_OF_MESSAGE_CLASSES)
    return (0);

  /* Class 0 is a special case that means 'all classes' */
  if (class == 0)
    {
      for (i=1; i<NUMBER_OF_MESSAGE_CLASSES; i++)
        m_is_class_enabled[i] = YES;
    }

  else
    m_is_class_enabled[class] = YES;

  return (1);
}



/*=r=*************************************************************************/
boolean message_class__disable (int class)
{
  int                     i;
  /*------------------------------------------------------------*/

  if (!m_initialized)
    m__initialize ();

  if (class >= NUMBER_OF_MESSAGE_CLASSES)
    return (0);

  /* Class 0 is a special case that means 'all classes' */
  if (class == 0)
    {
      for (i=1; i<NUMBER_OF_MESSAGE_CLASSES; i++)
        m_is_class_enabled[i] = NO;
    }

  else
    m_is_class_enabled[class] = NO;

  return (1);
}
