/*
**      Copyright (c) 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement. 
**
** cmdUtil -- A CCSDS Command utility. This program will build a CCSDS Command packet
**               with variable parameters and send it on a UDP network socket.
**               this program is primarily used to command a cFE flight software system.
**
*/


/*
** System includes
*/
#include <stdio.h>
#include <stdlib.h>
#include "getopt.h"
#include <string.h>
#include <limits.h>

/*
** sendUdp prototype -- Used to send the completed command to a UDP network socket.
*/
int SendUdp(char *hostname, char *portNum, char *packetData, int packetSize);

/*
** Defines
*/
#define DEFAULT_HOSTNAME "127.0.0.1"
#define HOSTNAME_SIZE     32

#define DEFAULT_PORTNUM  "1234"
#define PORTNUM_SIZE      10

#define PKTID_SIZE        10

#define DEFAULT_ENDIAN   "BE"
#define ENDIAN_SIZE       10
#define PACKETHDR_SIZE    (sizeof(short) * 4)
#define PACKETDATA_SIZE   512

#undef BIG_ENDIAN
#undef LITTLE_ENDIAN
#define BIG_ENDIAN    0
#define LITTLE_ENDIAN 1

/*
** Parameter datatype structure
*/
typedef struct {
    char HostName[HOSTNAME_SIZE]; /* Hostname like "localhost" or "192.168.0.121" */
    int  GotHostName;             /* did we get the hostname parameter ? */

    char PortNum[PORTNUM_SIZE]; /* Portnum: Default "1234" */
    int  GotPortNum;            /* did we get the port number parameter ? */

    char PktId[PKTID_SIZE];     /* PktId:  0x18xx : Required */
    int  GotPktId;              /* Did we get a PktID? */

    int  CmdCode;               /* Command code: Function code */
    int  GotCmdCode;            /* Did we get a command code? */

    int  Endian;                /* Swap parameter: BE or LE. Can be defaulted */
    int  GotEndian;             /* did we get the Endian parameter? */

    int  Verbose;               /* Verbose option set? */

    int  NumDataFields;         /* Number of data fields processed */
    int  DataLen;               /* total length of data fields */
    int  PacketLen;             /* Total packet length */

    char PacketHdr[PACKETHDR_SIZE];  /* 0x1804,0xC000,0x0001 */
    char PacketData[PACKETDATA_SIZE];/* Data buffer to build up parameters */
} CommandData_t;

/*
** Declare the global command data
*/
CommandData_t CommandData;
int hostByteOrder;

/*
** getopts parameter passing options string
*/
static const char *optString = "E:H:P:I:C:b:h:l:s:v?";

/*
** getopts_long long form argument table
*/
static struct option longOpts[] = {
    { "host",      required_argument, NULL, 'H' },
    { "port",      required_argument, NULL, 'P' },
    { "pktid",     required_argument, NULL, 'I' },
    { "cmdcode",   required_argument, NULL, 'C' },
    { "endian",    required_argument, NULL, 'E' },
    { "byte",      required_argument, NULL, 'b' },
    { "half",      required_argument, NULL, 'h' },
    { "long",      required_argument, NULL, 'l' },
    { "double",    required_argument, NULL, 'd' },
    { "string",    required_argument, NULL, 's' },
    { "help",      no_argument,       NULL, '?' },
    { "verbose",   no_argument,       NULL, 'v' },
    { NULL,        no_argument,       NULL, 0   }
};


/*
** IsLittleEndian -- This function returns 1 if the system uses little Endian
**                   byte-order, otherwise 0 for big Endian.
*/
int IsLittleEndian(void) {
    union {
        long l;
        unsigned char uc[sizeof(long)];
    } u;

    u.l = 1;
    return u.uc[0];
}

/*
* This function swaps bytes to the opposite byte-order format.
* It's limited to only swapping 16, 32, and 64-bit variable types.
*/
void byteSwap(char *byte, int num) {
    char temp;

    switch (num) {
        case 2:
            temp = byte[1];
            byte[1] = byte[0];
            byte[0] = temp;
            break;
        case 4:
            temp = byte[3];
            byte[3] = byte[0];
            byte[0] = temp;
            temp = byte[2];
            byte[2] = byte[1];
            byte[1] = temp;
            break;
        case 8:
            temp = byte[7];
            byte[7] = byte[0];
            byte[0] = temp;
            temp = byte[6];
            byte[6] = byte[1];
            byte[1] = temp;
            temp = byte[5];
            byte[5] = byte[2];
            byte[2] = temp;
            temp = byte[4];
            byte[4] = byte[3];
            byte[3] = temp;
            break;
        default:
            break;
    }
}


/*
** Display program usage, and exit.
*/
void DisplayUsage(char *Name )
{
    printf("%s -- A CCSDS Command Client.\n",Name);
    printf("      The parameters are:\n");
    printf("      --host : The hostname or IP address to send the command to ( default = localhost )\n");
    printf("      --port : The UDP port to send the command to ( default = 1234 )\n");
    printf("      --pktid : The Packet ID for the command being sent\n");
    printf("      --cmdcode : The command code for the command being sent\n");
    printf("      --endian : Target's byte-order ( BE or LE )\n");
    printf("      --half : Add a 16 bit parameter to the packet ( hex or dec )\n");
    printf("      --long : Add a 32 bit parameter to the packet ( hex or dec )\n");
    printf("      --double : Add a 64 bit parameter to the packet ( hex or dec )\n");
    printf("      --string : Add a fixed length string to the packet\n");
    printf("          The string parameter has the form \"NNN:StringData\"\n");
    printf("          Where NNN is the length of the parameter in the\n");
    printf("          command, and StringData is the string. So you could put\n");
    printf("          --string=\"32:ES_APP\" and it will build the string\n");
    printf("          parameter and pad the rest of the 32 bytes with 0's.\n");
    printf("      --byte : Add an 8 bit parameter to the packet ( hex or dec )\n");
    printf("          This one is not implemented yet, because I will have to\n");
    printf("          pad the other 8 bits..\n");
    printf(" \n");
    printf("       An example of using this is:\n");
    printf(" \n");
    printf("  ./cmdUtil --host=localhost --port=1234 --pktid=0x1803 --cmdcode=3 --half=100 --string=\"16:ES_APP\"\n");
    printf(" \n");

    exit( EXIT_FAILURE );
}


/*
** Process a string argument -- This function will add a padded string argument to the command data
**                              The string has to be of the form "NNN:String" Where NNN is a number
**                              specifying the total length of the command parameter ( because the
**                              command is not variable length ).
*/
void ProcessStringArgument(char *optarg, CommandData_t *CommandData) {
    char   stringLenString[10];
    int    stringIndex = 0;
    long   stringLength;

    while (optarg[stringIndex] != ':') {
        if (optarg[stringIndex] >= '0' && optarg[stringIndex] <= '9') {
            stringLenString[stringIndex] = optarg[stringIndex];
        } else {
            fprintf(stderr,"String Argument: '%s' rejected. Must be in the form: '000:String' where 000 is the length of the padded string\n",optarg);
            return;
        }
        stringIndex++;
        if (stringIndex > 3) {
            fprintf(stderr,"String Argument: '%s' rejected. The string length must be no more than 3 digits\n",optarg);
            return;
        }
    } /* end while */

    stringLenString[stringIndex] = '\0';
    stringLength = strtol(stringLenString, NULL, 10);
    if (CommandData->Verbose) {
        printf("String Length is %d.\n", stringLength);
    }

    if (stringLength > 128) {
        fprintf(stderr,"String Argument: '%s' rejected. The string length cannot be more than 128\n", optarg);
        return;
    }

    stringIndex++; /* go past the ':' character */
    strncpy(&CommandData->PacketData[CommandData->DataLen], (char*)&(optarg[stringIndex]), 128);

    CommandData->NumDataFields++;
    CommandData->DataLen += stringLength;
} /* End ProcessStringArgument */


/*
** Process a quad word (64-bit) argument
*/
void ProcessDoubleArgument(char *optarg, CommandData_t *CommandData) {
    double tempDouble;

    tempDouble = strtod(optarg, NULL);

    if (hostByteOrder != CommandData->Endian) {
        byteSwap((char*)&tempDouble, sizeof(tempDouble));
    }

    /* Pack the data as comes, which may not be aligned. */
    memcpy(&CommandData->PacketData[CommandData->DataLen], &tempDouble, sizeof(tempDouble));

    CommandData->NumDataFields++;
    CommandData->DataLen += 8;
}


/*
** Process a long word (32-bit) argument
*/
void ProcessLongArgument(char *optarg, CommandData_t *CommandData) {
    long tempLong;

    tempLong = strtol(optarg, NULL, 0);

    if (hostByteOrder != CommandData->Endian) {
        byteSwap((char*)&tempLong, sizeof(tempLong));
    }

    /* Pack the data as comes, which may not be aligned. */
    memcpy(&CommandData->PacketData[CommandData->DataLen], &tempLong, sizeof(tempLong));

    CommandData->NumDataFields++;
    CommandData->DataLen += 4;
}

/*
** Process a half word (16-bit) argument
*/
void ProcessHalfArgument(char *optarg, CommandData_t *CommandData) {
    long  tempLong;
    short tempShort;

    tempLong = strtol(optarg, NULL, 0);
    if (tempLong > 0xFFFF) {
        fprintf(stderr, "Half-Word Argument: '%s' rejected. Number is too large.\n", optarg);
        return;
    }

    tempShort = (short)tempLong;
    if (hostByteOrder != CommandData->Endian) {
        byteSwap((char*)&tempShort, sizeof(tempShort));
    }

    /* Pack the data as comes, which may not be aligned. */
    memcpy(&CommandData->PacketData[CommandData->DataLen], &tempShort, sizeof(tempShort));

    CommandData->NumDataFields++;
    CommandData->DataLen += 2;
}

/*
** Process a Byte (8-bit) argument
*/
void ProcessByteArgument(char *optarg, CommandData_t *CommandData) {
    long tempLong;
    char tempByte;

    tempLong = strtol(optarg, NULL, 0);
    if (tempLong > 0xFF) {
        fprintf(stderr, "Byte Argument: '%s' rejected. Number is too large.\n", optarg);
        return;
    }

    tempByte = (char)tempLong;
    CommandData->PacketData[CommandData->DataLen] = tempByte;

    CommandData->NumDataFields++;
    CommandData->DataLen += 1;
}

/*
** ProcessArgumentDefaults -- This function assigns defaults to parameters and checks to make sure
**                             the user entered required parameters
*/
void ProcessArgumentDefaults(CommandData_t *CommandData) {
    if (CommandData->GotHostName == 0) {
        strncpy(CommandData->HostName, DEFAULT_HOSTNAME, HOSTNAME_SIZE);
        if (CommandData->Verbose){
            printf("No Hostname Given, defaulting to %s\n", DEFAULT_HOSTNAME);
        }
        CommandData->GotHostName = 1;
    }

    if (CommandData->GotPortNum == 0) {
        strncpy(CommandData->PortNum, DEFAULT_PORTNUM, PORTNUM_SIZE);
        if (CommandData->Verbose) {
            printf("No Port Number Given, defaulting to %s\n",DEFAULT_PORTNUM);
        }
        CommandData->GotPortNum = 1;
    }

    if (CommandData->GotEndian == 0) {
        if (CommandData->Verbose) {
            printf("No Endian option given, defaulting to %s\n", DEFAULT_ENDIAN);
            CommandData->GotEndian = 1;
        }
    }

    /*
    ** Make sure we got a minimum set of options passed in
    */
    if (CommandData->GotPktId == 0) {
        fprintf(stderr,"Error: Need to specify a Packet ID using the --pktid or -I option.\n");
        exit(EXIT_FAILURE);
    }

    if (CommandData->GotCmdCode == 0) {
        fprintf(stderr,"Error: Need to specify a Command Code using the --cmdcode or -C option.\n");
        exit(EXIT_FAILURE);
    }

}


/*
** main function
*/
int main(int argc, char *argv[]) {
    int   opt = 0;
    int   longIndex = 0;
    int   retStat;
    long  tempLong;
    short tempShort;

    /*
    ** Initialize the CommandData struct
    */
    memset(&(CommandData), 0, sizeof(CommandData_t));

    hostByteOrder = IsLittleEndian();

    /*
    ** Process the arguments with getopt_long(), then
    ** Build the packet.
    */
    opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
    while( opt != -1 )
    {
        switch( opt )
        {
        case 'H':
            printf("Host: %s\n",(char *)optarg);
            strncpy(CommandData.HostName, optarg, HOSTNAME_SIZE);
            CommandData.GotHostName = 1;
            break;

        case 'P':
            printf("Port: %s\n",(char *)optarg);
            strncpy(CommandData.PortNum, optarg, PORTNUM_SIZE);
            CommandData.GotPortNum = 1;
            break;

        case 'I':
            printf("Pkt ID: %s\n", (char*)optarg);
            tempShort = (short)strtol(optarg, 0, 0);

            /* 
            ** The packet ID is supposed to be in Big Endian order 
            ** If this is running on a little endian host, swap it 
            */
            if (hostByteOrder == LITTLE_ENDIAN )
            {
                byteSwap((char*)&tempShort, sizeof(tempShort));
            }
            memcpy(&CommandData.PacketHdr[0], &tempShort, sizeof(tempShort));
            CommandData.GotPktId = 1;
            break;

        case 'v':
            printf("Verbose messages on.\n");
            CommandData.Verbose = 1;
            break;

        case 'C':
            tempShort = strtol(optarg, NULL, 0);
            if (tempShort > 0x7F) {
                fprintf(stderr,"Command Code Argument: '%s' rejected. Number is too large.\n",optarg);
                break;
            }
            /* 
            ** Shift the command code to the upper 8 bits of the word 
            */
	    tempShort <<= 8;

            /*
            ** Swap if needed 
            */
            if (hostByteOrder != CommandData.Endian) 
            {
               byteSwap((char*)&tempShort, sizeof(tempShort));
            }

            memcpy(&CommandData.PacketHdr[6], &tempShort, sizeof(tempShort));
            CommandData.GotCmdCode = 1;
            break;

        case 'E':
            if (strncmp(optarg, DEFAULT_ENDIAN, ENDIAN_SIZE) != 0) {
                CommandData.Endian = LITTLE_ENDIAN;
            }
            CommandData.GotEndian = 1;
            break;

        case 'b':
            ProcessByteArgument(optarg, &CommandData);
            break;

        case 'h':
            ProcessHalfArgument(optarg, &CommandData);
            break;

        case 'l':
            ProcessLongArgument(optarg, &CommandData);
            break;

        case 'd':
            ProcessDoubleArgument(optarg, &CommandData);
            break;

        case 's':
            ProcessStringArgument(optarg, &CommandData);
            break;

        case '?':
            DisplayUsage(argv[0]);
            break;

        default:
            break;
        }

        opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
    }

    /*
    ** Set the defaults for values that were not given for the
    ** optional arguments, and check for arguments that are required.
    */
    ProcessArgumentDefaults(&CommandData);

    /*
    ** Assemble the packet header
    */
    tempShort = 0xC000; /* Sequence word. */
    if (hostByteOrder == LITTLE_ENDIAN)
    {
        byteSwap((char*)&tempShort, sizeof(tempShort));
    }
    memcpy(&CommandData.PacketHdr[2], &tempShort, sizeof(tempShort));

    /* 
    ** Process size field in header
    ** Size field needs to be Size of packet data not including cmd secondary hdr + 1
    */
    tempShort = CommandData.DataLen + 1;
    if (hostByteOrder == LITTLE_ENDIAN)
    {
        byteSwap((char*)&tempShort, sizeof(tempShort));
    }
    memcpy(&CommandData.PacketHdr[4], &tempShort, sizeof(tempShort));

    /*
    ** Send the packet
    */
    retStat = SendUdp(CommandData.HostName, CommandData.PortNum, CommandData.PacketHdr, CommandData.DataLen + 8); 

    if (retStat < 0) 
    {
        fprintf(stderr,"Problem sending UDP packet: %d\n", retStat);
        return ( retStat );
    } 
    else if (CommandData.Verbose) 
    {
        printf("Command packet sent OK.\n");
    }

    return EXIT_SUCCESS;
}
