#!/usr/bin/env python
#

import sys
import os
import socket
import zmq

from PyQt4 import QtGui, QtNetwork, QtCore
from struct import *
from time import sleep

# Receive port where the CFS TO_Lab app sends the telemetry packets
udpRecvPort = 1235

#
# Receive telemetry packets, apply the appropiate header
# and publish the message with zeroMQ
#
class RoutingService(QtCore.QThread):

    def __init__(self, mainWindow):
        QtCore.QThread.__init__(self)
        # Signal to update the spacecraft combo box (list) on main window GUI
        self.signalUpdateIpList = QtCore.SIGNAL("changeIpList")

        # Init lists
        self.ipAddressesList = ["All"]
        self.spacecraftNames = ["All"]
        self.specialPktId = []
        self.specialPktName = []

        # Init zeroMQ
        self.context   = zmq.Context()
        self.publisher = self.context.socket(zmq.PUB)
        self.publisher.bind("ipc:///tmp/GroundSystem")

    # Run thread
    def run(self):
        # Init udp socket
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind(('', udpRecvPort))

        # Wait for UDP messages
        while True:
            try:
                # Receive message
                datagram, host = self.sock.recvfrom(4096) # buffer size is 1024 bytes

                # Ignore datagram if it is not long enough (doesnt contain tlm header?)
                if len(datagram) < 6:
                    continue

                # Read host address
                hostIpAddress = host[0]

                #
                # Add Host to the list if not already in list
                #
                if not any(hostIpAddress in s for s in self.ipAddressesList):
                    hostName = "Spacecraft" + str(len(self.spacecraftNames))
                    print "Detected " + hostName + " at " + hostIpAddress
                    self.ipAddressesList.append(hostIpAddress);
                    self.spacecraftNames.append(hostName)
                    self.emit(self.signalUpdateIpList, hostIpAddress, hostName)

                # Forward the message using zeroMQ
                name = self.spacecraftNames[self.ipAddressesList.index(hostIpAddress)]
                self.forwardMessage(datagram, name)

            # Handle errors
            except socket.error, v:
                print 'Ignored socket error.'
                sleep(1)

    # Apply header using hostname and packet id and send msg using zeroMQ
    def forwardMessage(self, datagram, hostName):
        # Forward message to channel GroundSystem.<Hostname>.<pktId>
        pktId = self.getPktId(datagram)
        header = "GroundSystem." + hostName + ".TelemetryPackets." + pktId
        self.publisher.send_multipart([header, datagram])
        #print header


    # Read the packet id from the telemetry packet
    def getPktId(self, datagram):
        # Read the telemetry header
        streamId, Sequence, Length = unpack(">HHH",datagram[:6])
        # Uncomment the next line to debug
        # print "Packet ID = " , hex(streamId)
        return hex(streamId)

    # Close ZMQ vars
    def stop(self):
        self.sock.close()
        self.context.destroy()


