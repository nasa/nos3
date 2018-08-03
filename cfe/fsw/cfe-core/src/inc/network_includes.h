/******************************************************************************
** File: network_includes.h
** $Id: network_includes.h 1.3 2009/07/31 10:20:30GMT-05:00 acudmore Exp  $
**
** Purpose:
**   This header file contains the correct set of network include
**   files, which are dependant on the operating system.
**
** Author:   R.McGraw/SSI
**
** Notes:
**
** $Date: 2009/07/31 10:20:30GMT-05:00 $
** $Revision: 1.3 $
** $Log: network_includes.h  $
** Revision 1.3 2009/07/31 10:20:30GMT-05:00 acudmore 
** Updated RTEMS section to define a MSG_DONTWAIT flag that ci_lab uses.
** Revision 1.2 2009/07/17 14:18:09EDT acudmore 
** Updated includes to prevent warnings from ARC suggestions.
** Revision 1.1 2008/04/17 08:05:27EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/inc/project.pj
** Revision 1.5 2006/06/08 14:20:29EDT apcudmore 
** Re-ordered include files to prevent warnings/errors for vxWorks and REMS
** Revision 1.4 2006/03/03 16:28:38GMT apcudmore 
** Created conditional compilation for systems that dont have the fcntl function
** Revision 1.3 2006/03/03 15:11:03GMT apcudmore 
** Fixed non-blocking socket problem with CI
** Revision 1.2 2005/07/29 18:16:40GMT apcudmore 
** Updated for mac and linux builds
** Revision 1.1 2005/06/09 09:57:51CDT rperera 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-REPOSITORY/cfe-core/inc/project.pj
** Revision 1.1  2005/03/23 16:04:15  rmcgraw
** Initial Release
**
**
******************************************************************************/

#ifndef _network_includes_
#define _network_includes_

#ifdef _VXWORKS_OS_
    #include <vxWorks.h>
    #include <selectLib.h>
    #include <sockLib.h>
    #include <inetLib.h>
    #include <arpLib.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>

#elif  _RTEMS_OS_
    #define _USING_RTEMS_INCLUDES_
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <fcntl.h>
    #define _HAVE_FCNTL_
    #ifndef MSG_DONTWAIT
      #define MSG_DONTWAIT 0
    #endif
#elif  _MAC_OS_
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <fcntl.h>
    #define _HAVE_FCNTL_

#elif  _LINUX_OS_
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <fcntl.h>
    #define _HAVE_FCNTL_
    #ifndef MSG_DONTWAIT
      #define MSG_DONTWAIT 0
    #endif

#else
    #error "No OS defined!"
#endif

#endif /* _network_includes_ */
