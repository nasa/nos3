/**
 * @file
 * 
 * This file contains all functions relevant to reading/writing the serial 
 * device. 
 * 
 * @author Jaclyn Beck, Jonathan Urriste
 * @date 2015/06/24 15:30:00
 */
#include "serial_io.h"
#include "serial_queue.h"
#include "sbn_constants.h"
#include "serial_events.h"
#include "serial_sbn_if_struct.h"
#include <arpa/inet.h>
#include <string.h>

#ifdef SBN_SERIAL_USE_TERMIOS
#include <errno.h>
#include <termios.h> 
#include <unistd.h>
#endif

/* TODO: move to HostData */
uint32 HostQueueId = 0; 

/**
 * Opens the serial port as a file descriptor and then sets up some settings 
 * like baud rate. 
 * 
 * @param DevName   The name of the device to open (e.g. "/dev/ttyS0")
 * @param BaudRate  The desired baud rate of the serial port
 * @param Fd        Pointer to the file descriptor int
 *
 * @return SBN_OK on success, SBN_ERROR on error
 */
int32 Serial_IoOpenPort(char *DevName, uint32 BaudRate, int32 *Fd)
{
    /* open serial device and set options */
    *Fd = OS_open(DevName, OS_READ_WRITE, 0); 
    if(*Fd < 0)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_IO_EID, CFE_EVS_ERROR,
            "Serial: Error opening device %s. Returned %d\n", DevName, *Fd);
        return SBN_ERROR;
    }/* end if */
    if(Serial_IoSetAttrs(*Fd, BaudRate) == SBN_ERROR)
    {
        OS_close(*Fd);
        return SBN_ERROR;
    }/* end if */

    return SBN_OK;
}/* end Serial_IoOpenPort */


#ifdef SBN_SERIAL_USE_TERMIOS

/**
 * Specifies the TTY settings for the serial interface. This function requires
 * that the OS supports termios. 
 *
 * @param Fd        The file descriptor for the serial TTY device
 * @param Baud      Desired baud, in terms of constants in termios.h
 * @param Parity    If no parity is desired, set to 0
 *
 * @return SBN_OK if successful
 * @return SBN_ERROR if unsuccessful
 */
int32 Serial_IoSetAttrs(int32 Fd, uint32 BaudRate)
{
    struct termios tty;
    int32 termiosBaud; 
    OS_FDTableEntry tblentry;

    OS_FDGetInfo(Fd, &tblentry);

    if(tcgetattr(tblentry.OSfd, &tty) != 0)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_IO_EID, CFE_EVS_ERROR,
            "Serial: Error accessing tty settings, errno: 0x%x\n", errno);
        return SBN_ERROR;
    }/* end if */

    switch(BaudRate)
    {
        case 38400:
            termiosBaud = B38400;
            break;

        case 57600:
            termiosBaud = B57600;
            break;

        case 115200:
            termiosBaud = B115200;
            break;
        
        case 230400:
            termiosBaud = B230400;
            break;

        default:
            CFE_EVS_SendEvent(SBN_SERIAL_IO_EID, CFE_EVS_ERROR,
                "Serial: Unknown baud rate %d\n", BaudRate); 
            return SBN_ERROR; 
    }/* end switch */

    cfsetospeed(&tty, termiosBaud); /* Set output baud rate */
    cfsetispeed(&tty, termiosBaud); /* Set input baud rate */
	
    /* 8 bit words */
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    /* disable break processing */
    tty.c_iflag &= ~IGNBRK;
    /* disable signaling characters, echo, canonical processing */
    tty.c_lflag = 0;
    /* disable remapping and delays */
    tty.c_oflag = 0;
    /* disable xon/xoff control */
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    /* disable modem controls, enable reading */
    tty.c_cflag |= (CLOCAL | CREAD);
    /* disable parity */
    tty.c_cflag &= ~(PARENB | PARODD);
    /* send 1 stop bit */
    tty.c_cflag &= ~CSTOPB;
#ifdef CRTSCTS /* LINUX doesn't support unless _BSD_SOURCE defined */
    /* no flow control */
    tty.c_cflag &= ~CRTSCTS;
#endif

    /* Don't block until a character has been received */
    tty.c_cc[VMIN]  = 0;
    /* read() will timeout after 10 tenths of a second */
    tty.c_cc[VTIME] = 10;

    tcflush(tblentry.OSfd, TCIFLUSH);
    if(tcsetattr(tblentry.OSfd, TCSANOW, &tty) != 0)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_IO_EID, CFE_EVS_ERROR,
            "Serial: Error setting tty settings, errno: 0x%x\n", errno);
        return SBN_ERROR;
    }/* end if */

    return SBN_OK;
}/* end Serial_IoSetAttrs */

#else /* !SBN_SERIAL_USE_TERMIOS */

/**
 * Non-Linux OS / non termios implementation of setting serial settings. 
 */
int32 Serial_IoSetAttrs(int32 Fd, uint32 BaudRate)
{
    CFE_EVS_SendEvent(SBN_SERIAL_IO_EID, CFE_EVS_ERROR,
        "Serial: Serial_IoSetAttrs not implemented for this OS\n"); 
    return SBN_ERROR;
}/* end Serial_IoSetAttrs */

#endif /* SBN_SERIAL_USE_TERMIOS */

/**
 * Tries to read a message off the serial wire. If a message is read, it
 * determines which queue to put it in and adds the message to that queue. Each
 * message starts with a 4-byte sync word followed by the message length
 * (which includes the sync word, message length bytes, and the message payload)
 * and the payload. 
 *
 * @param host   The host data struct containing queues and the file descriptor
 *
 * @return dataRead (number of bytes read off the wire)
 * @return SBN_IF_EMPTY if no message to read
 * @return SBN_ERROR if unsuccessful
 */
int32 Serial_IoReadMsg(Serial_SBNHostData_t *Host)
{
    int32 Received = 0, TotalReceived = 0;
    uint8 MsgBuf[SBN_MAX_MSG_SIZE];
    SBN_MsgSize_t MsgSize;
    SBN_MsgType_t MsgType;
    SBN_CpuId_t CpuId;

    /* read the SBN header, which includes a message size so we know how
     * much to read to get the rest of the message */

    TotalReceived = 0;

    Received = OS_read(Host->Fd, &MsgSize, sizeof(MsgSize));
    if(Received < sizeof(MsgSize))
    {
        CFE_EVS_SendEvent(SBN_SERIAL_IO_EID, CFE_EVS_ERROR,
            "Serial: Unable to read the message header.");
        return SBN_ERROR;
    }
    TotalReceived += Received;

    MsgSize = ntohs(MsgSize);

    Received = OS_read(Host->Fd, &MsgType, sizeof(MsgType));
    if(Received < sizeof(MsgType))
    {
        CFE_EVS_SendEvent(SBN_SERIAL_IO_EID, CFE_EVS_ERROR,
            "Serial: Unable to read the message header.");
        return SBN_ERROR;
    }
    TotalReceived += Received;

    Received = OS_read(Host->Fd, &CpuId, sizeof(CpuId));
    if(Received < sizeof(CpuId))
    {
        CFE_EVS_SendEvent(SBN_SERIAL_IO_EID, CFE_EVS_ERROR,
            "Serial: Unable to read the message header.");
        return SBN_ERROR;
    }
    TotalReceived += Received;

    CpuId = ntohl(CpuId);

    TotalReceived = 0;
    while(TotalReceived < MsgSize)
    {
        Received = OS_read(Host->Fd, MsgBuf + TotalReceived,
            sizeof(MsgBuf) - TotalReceived);
        if(Received < 0)
        {
            CFE_EVS_SendEvent(SBN_SERIAL_IO_EID, CFE_EVS_ERROR,
                "Serial: Unable to read the message.");
            return SBN_ERROR;
        }/* end if */
        TotalReceived += Received;
    }/* end while */

    return Serial_QueueAddNode(Host->Queue, Host->SemId, MsgType, MsgSize,
        CpuId, MsgBuf);
}/* end Serial_IoReadMsg */

/**
 * Thread that continuously reads the serial device and puts the read messages
 * in the queue.
 */
void Serial_IoReadTaskMain()
{
    int32 dataRead = 0;
    Serial_SBNHostData_t *Host;
    uint32 size; 

    CFE_ES_RegisterChildTask();

    OS_QueueGet(HostQueueId, &Host, sizeof(uint32), &size, OS_PEND); 

    if(size == 0 || Host == NULL || Host->Fd < 0)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_IO_EID, CFE_EVS_ERROR,
            "Serial: Cannot start read task. Host is null.\n"); 
        CFE_ES_ExitChildTask();
        return;
    }/* end if */

    /* Keep reading forever unless there's an error */
    while(dataRead == SBN_IF_EMPTY || dataRead >= 0)
    {
        dataRead = Serial_IoReadMsg(Host); 
    }/* end while */

    CFE_EVS_SendEvent(SBN_SERIAL_IO_EID, CFE_EVS_INFORMATION,
        "Serial: Serial Read Task exiting for host number %d\n", Host->PairNum);
    CFE_ES_ExitChildTask();
}/* end Serial_IoReadTaskMain */


/**
 * You cannot pass arguments to threads in CFE so this function puts the host in
 * an OS queue accessible across threads, starts a task, and the task pulls the 
 * host off the queue when it starts up. This function returns TRUE or 
 * FALSE because called during host validation. An error here means the
 * host is not valid and shouldn't be used.
 *
 * @param host  The host to start reading from
 *
 * @return TRUE on success
 * @return FALSE on error
 */
int32 Serial_IoStartReadTask(Serial_SBNHostData_t *Host)
{
    char name[20]; 
    int32 Status; 

    /* If the queue doesn't already exist, create it */
    if(HostQueueId == 0)
    {
        Status = OS_QueueCreate(&HostQueueId, "SerialHostQueue",
            SBN_SERIAL_QUEUE_DEPTH,
            sizeof(uint32), 0); 

        if(Status != OS_SUCCESS)
        {
            CFE_EVS_SendEvent(SBN_SERIAL_IO_EID, CFE_EVS_INFORMATION,
                "Serial: Error creating host queue. Returned %d\n", Status); 
            return FALSE;
        }/* end if */
    }/* end if */

    /* Add this host to the queue */
    Status = OS_QueuePut(HostQueueId, &Host, sizeof(uint32), 0); 
    if(Status != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_IO_EID, CFE_EVS_INFORMATION,
            "Serial: Error adding host to queue. Returned %d\n", Status); 
        return FALSE; 
    }/* end if */

    /* Start the child task that will read this host's fd */
    sprintf(name, "SerialReadTask%d", Host->PairNum); 
    Status = CFE_ES_CreateChildTask(&Host->TaskHandle,
        name, Serial_IoReadTaskMain, NULL, SBN_SERIAL_CHILD_STACK_SIZE,
        SBN_SERIAL_CHILD_TASK_PRIORITY, 0);

    if(Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_IO_EID, CFE_EVS_INFORMATION,
            "Serial: Error creating read task for host %d. Returned %d\n", 
            Host->PairNum, Status); 
        return FALSE; 
    }/* end if */

    return TRUE; 
}/* end Serial_IoStartReadTask */
