FILE:    README
AUTHOR:  Jaclyn Beck/587
REVISED: 06/06/2014

-------------------------------------------------------------------------
0. Contents
-------------------------------------------------------------------------
1. General Description
2. Constraints
   A) Data Format
   B) Device Consistency
3. Shared Memory Structure
   A) Layout
   B) Queue Operation Notes

-------------------------------------------------------------------------
1. General Description
-------------------------------------------------------------------------

The shared memory plugin for the Software Bus Network (SBN) allows two CPUs 
running cFE to send software bus (SB) messages to each other through shared 
memory. This plugin is never used directly by a user cFE app; instead it is used
by the SBN to make message passing across CPUs transparent to other apps. 

SBN uses this module by internally calling a set of functions:
- SBN_ShMemParseFileEntry        (Reads module setup from SBN peer data file)
- SBN_ShMemInitIF                (Initializes shared memory interface)
- SBN_ShMemSendNetMsg            (Sends SB or SBN messages to other CPU)
- SBN_ShMemCheckForNetProtoMsg   (Checks for SBN protocol messages)
- SBN_ShMemRcvMsg                (Reads software bus messages from other CPU)
- SBN_ShMemVerifyPeerInterface   (Makes sure peer entry has matching host)
- SBN_ShMemVerifyHostInterface   (Makes sure host entry has matching peer)

These functions then use shared memory-specific read/write operations to 
pass messages. 

-------------------------------------------------------------------------
2. Constraints
-------------------------------------------------------------------------

A) Data Format
- The maximum message size that can be sent over the Software Bus Network is 
  1400 bytes. 
- The shared memory plugin is currently written for Linux only and other
  operating systems will not be able to use it. 
- When defining the address and size of shared memory in the SBN peer data file,
  the address of each memory segment must be on a page boundary and the size
  of that memory segment must be a multiple of page size. 

B) Device Consistency
- Shared memory is erased each time SBN starts the plugin on either CPU.
  Messages that were written to shared memory by one CPU will be lost if the 
  other CPU starts SBN after those messages were written. 

-------------------------------------------------------------------------
3. Shared Memory Structure
-------------------------------------------------------------------------

A) Layout
The shared memory plugin expects the shared memory to be divided into four 
regions per CPU:
	1. Data Receive
	2. Data Send
	3. Protocol Receive
	4. Protocol Send
Each CPU's send regions are the other CPU's receive regions, and vice versa. 

Each region has the following structure (with sizes in bytes):

|---------- Header ----------|---------------- Message Queue ----------------...|
---------------------------------------------------------------------------------
| Mutex | ReadPtr | WritePtr | Msg1Len |   Msg 1    | Msg2Len |   Msg 2   | ... |
---------------------------------------------------------------------------------
  (24)      (4)       (4)        (2)     (Msg1Len)      (2)     (Msg2Len)

- The mutex locks and unlocks the region for reading/writing. 
- ReadPtr points to the location of the length of the first message to be read.
- WritePtr points to the location of the first empty place in memory to write. 
- The messages are read/written as a FIFO to allow for queueing of messages. 

B) Queue Operation Notes
- ReadPtr and WritePtr are relative to the start address of the message queue.
  That is, if the memory segment starts at address 0xfffd0000, the message queue
  starts at 0xfffd0020, and the next message to read is at address 0xfffd0094, 
  ReadPtr would be set to 116 (0x74). This is done because the two CPUs don't 
  necessarily map shared memory to the same virtual location so the pointers can 
  not be absolute locations. 
- When WritePtr gets large enough that a new message cannot be written without 
  overflowing the memory segment, it first writes the length of the message at 
  its current location and then wraps back to the start of the message queue to
  write the message there. When ReadPtr encounters this length that can't fit,
  it wraps to the beginning of the message queue and reads the message there 
  instead. 
- If writing a new message will overwrite part of the next message in the read 
  queue (possibly invalidating the value in the "length" field), ReadPtr is 
  moved to the next message in the read queue to make room and the message about 
  to be overwritten is lost.

