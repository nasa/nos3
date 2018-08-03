#ifndef _serial_sbn_if_struct_h_
#define _serial_sbn_if_struct_h_

#include "cfe.h"
#include "serial_platform_cfg.h"


/** \brief SBN Peer File entry data */
typedef struct {
    char    DevNameHost[SBN_SERIAL_MAX_CHAR_NAME]; /**< Name of the device on the operating system, e.g. '/dev/ttyS0' */
    uint8   PairNum;    /**< For identifying which peer/host this entry is paired with */
    uint32  BaudRate;   /**< Baud rate of the serial device */
} Serial_SBNEntry_t;


/** \brief Host data */
typedef struct {
    char    DevName[SBN_SERIAL_MAX_CHAR_NAME]; /**< Name of the device on the operating system, e.g. '/dev/ttyS0' */
    uint32  PairNum;        /**< For identifying which peer this host is paired with */
    uint32  BaudRate;       /**< Baud rate of the serial device */
    int32   Fd;             /**< File descriptor for TTYserial device */
    uint32  Queue;          /**< Data message queue ID */
    uint32  SemId;          /**< Semaphore for data queue */
    uint32  TaskHandle;     /**< Handle for the serial read task */
} Serial_SBNHostData_t;


/** \brief Peer data */
typedef struct {
    uint32  PairNum;        /**< For identifying the host this peer is paired with */
    uint32  BaudRate;       /**< Baud rate of the serial device */
} Serial_SBNPeerData_t;


/**
 * \brief Module status data 
 *
 * This struct is what gets reported in telemetry by SBN's "ReportModuleStatus" 
 * function. 
 * Note: This struct is not packed because doxygen can't handle having OS_PACK
 * in front of the struct name, but PeerData should appear where it's expected
 * to start because the host data struct is 64 bytes. 
 */
typedef struct {
    Serial_SBNHostData_t HostData;
    Serial_SBNPeerData_t PeerData; 
} Serial_SBNModuleStatus_t; 

#endif /* _serial_sbn_if_struct_h_ */
