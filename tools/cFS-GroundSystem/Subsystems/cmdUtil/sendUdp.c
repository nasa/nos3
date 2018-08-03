/*
**      Copyright (c) 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement. 
**
**      Udp packet send routine
*/

#ifdef WIN32
    #pragma warning (disable:4786)
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    typedef int socklen_t;
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <ctype.h>
    #include <stdio.h>
    #include <string.h>
    #define SOCKET int
    #define closesocket(fd) close(fd)
#endif

/*
** SendUdp
*/
int SendUdp(char *hostname, char *portNum, char *packetData, int packetSize) {
    SOCKET              sd;
    int                 rc;
    unsigned int        i;
    struct sockaddr_in  cliAddr;
    struct sockaddr_in  remoteServAddr;
    struct hostent     *hostID;
    int                 port;

    #ifdef WIN32
        WSADATA  wsaData;
        WSAStartup(WINSOCK_VERSION, &wsaData);
    #endif

    if (hostname == NULL) {
        return -1;
    }

    /*
    ** get server IP address (no check if input is IP address or DNS name
    */
    hostID = gethostbyname(hostname);
    if (hostID == NULL) {
        return -2;
    }

    /*
    ** Check port
    */
    port = atoi(portNum);
    if (port == -1) {
        return -3;
    }

    printf("sending data to '%s' (IP : %s); port %d\n", hostID->h_name,
        inet_ntoa(*(struct in_addr *)hostID->h_addr_list[0]), port);

    /*
    ** Setup socket structures
    */
    remoteServAddr.sin_family = hostID->h_addrtype;
    memcpy((char *) &remoteServAddr.sin_addr.s_addr,
        hostID->h_addr_list[0], hostID->h_length);
    remoteServAddr.sin_port = htons(port);

    /*
    ** Create Socket
    */
    sd = socket(AF_INET,SOCK_DGRAM,0);
    if (sd < 0) {
        return -4;
    }

    /*
    ** bind any port
    */
    cliAddr.sin_family = AF_INET;
    cliAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    cliAddr.sin_port = htons(0);

    rc = bind(sd, (struct sockaddr *) &cliAddr, sizeof(cliAddr));
    if (rc < 0) {
        printf("%s: cannot bind port\n", portNum);
        return -5;
    }

    printf("Data to send:\n");
    i = 0;
    while (i < packetSize) {
        printf("0x%02X ", packetData[i] & 0xFF);
        if (++i % 8 == 0) {
            puts("");
        }
    }
    puts("");

    /*
    ** send the event
    */
    rc = sendto(sd, (char*)packetData, packetSize, 0,
        (struct sockaddr*)&remoteServAddr,
        sizeof(remoteServAddr));

    if (rc < 0) {
        closesocket(sd);
        return -6;
    }

    closesocket(sd);
    return 0;
}
