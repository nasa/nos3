#!/usr/bin/env python
#

#
# Send UDP messages for debugging (not used by Ground System)
#
import sys
import time
from socket import *

#
# Main
#
if __name__ == "__main__":
    num = 0
    while True:
        num += 1
        time.sleep(1)
#        send_host = "10.1.57.37"
        send_host = "127.0.0.1"
#        send_host = "192.168.1.4"
        send_port =  1235
        datagram = 'Test tlm message'
        sendSocket = socket(AF_INET,SOCK_DGRAM)
        sendSocket.sendto(datagram, (send_host,send_port))
        print 'Sent msg #' + str(num)

