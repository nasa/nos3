$Log: readme.txt  $
Revision 1.2 2009/07/30 11:06:42GMT-05:00 acudmore 
Updated cmdUtil submitted by NASA ARC
Revision 1.1 2008/04/17 08:08:42EDT ruperera 
Initial revision
Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/cmdUtil/project.pj
Revision 1.1 2007/04/06 15:24:44EDT apcudmore 
Initial revision
Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/cmdUtil/project.pj

cmdUtil is a command line C program that is designed to run on a host computer
( development machine, ground system, test system, etc.. ) and send commands to a
CCSDS system over a UDP network socket.

This utility will format and send a CCSDS command packet over a UDP network socket. It 
can be used to send commands on a localhost, or a remote system. It is primarily used for
cFE ( core Flight Executive ) development and testing.

Documentation for the cmdUtil Program:

The following is the help text from cmdUtil:
cmdUtil -- A CCSDS Command Client.
      The parameters are:
      --host : The hostname or IP address to send the command to ( default = localhost)
      --port : The UDP port to send the command to ( default = 1234 )
      --pktid : The Packet ID for the command being sent
      --cmdcode : The command code for the command being sent
      --endian : BE: Dont swap words, LE: Swap words on packet
      --half : Add a 16 bit parameter to the packet ( hex or dec )
      --long : Add a 32 bit parameter to the packet ( hex or dec )
      --string : Add a fixed length string to the packet
          The string parameter has the form "NNN:StringData"
          Where NNN is the length of the parameter in the
          command, and StringData is the string. So you could put
          --string="32:ES_APP" and it will build the string
          parameter and pad the rest of the 32 bytes with 0's.
      --byte : Add an 8 bit parameter to the packet ( hex or dec )
          This one is not implemented yet, because I will have to
          pad the other 8 bits..
 
       An example of using this is:
 
  ./cmdUtil --host=localhost --port=1234 --pktid=0x1803 --cmdcode=3 --half=100 --string="16:ES_APP"

There are a couple of example shell scripts to show how the cmdUtil program can be invoked:
The to-enable-tlm.sh is a script that sends a single command to the Telemetry
Output (TO) application.

The user can modify a copy of the to-enable-tlm.sh script to include the
desired commands to be sent.

To send the commands, just execute the script at the command prompt like this:
$./to-enable-tlm.sh

