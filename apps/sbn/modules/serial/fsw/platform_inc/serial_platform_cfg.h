/**
 * @file
 * 
 * This file contains several user-configurable parameters
 * 
 * @author Jaclyn Beck
 * @date 2015/06/24 15:30:00
 */
#ifndef _serial_platform_cfg_h_
#define _serial_platform_cfg_h_

#define SBN_SERIAL_QUEUE_DEPTH          20  /**< Max of 20 messages in the queue before data gets dropped. 
                                                 This variable is ignored in the function OS_QueueCreate though. */

#define SBN_SERIAL_ITEMS_PER_FILE_LINE  3   /**< How many items are in the SbnPeerData file's line description for this module */

#define SBN_SERIAL_MAX_CHAR_NAME        32  /**< How long the device name can be in the SbnPeerData file */

#define SBN_SERIAL_CHILD_STACK_SIZE     2048 /**< Stack size that each child task gets */

#define SBN_SERIAL_CHILD_TASK_PRIORITY  70  /**< Priority of the child tasks */

#define SBN_SERIAL_HOST_QUEUE_NAME      "HostQueue" /**< Queue that holds valid hosts for the child tasks to use */

#define SBN_SERIAL_USE_TERMIOS  /**< If defined, code will use termios to set serial settings. If
                                     this module is running on an OS that doesn't support termios
                                     (non Linux/POSIX), undefine this and fill in the function
                                     "Serial_IoSetAttrs" for your OS */

#endif
