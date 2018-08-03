#!/usr/bin/env python
#
# EVS Events page
# 
# The EVS Event Message has the following format
# 
#
# ES HK format:
#
# Packet Header
#   uint16  StreamId;   0   
#   uint16  Sequence;   2
#   uint16  Length;     4 

# Tlm Secondary Header
#   uint32  seconds     6
#   uint16  subseconds  10

#  Packet data
#
# Event format:
#
# Packet Header
#   uint16  StreamId;    
#   uint16  Sequence; 
#   uint16  Length;   

# Tlm Secondary Header
#   uint32  seconds
#   uint16  subseconds

# Packet ID
#   char    AppName[20] 
#   uint16  EventID;        
#   uint16  EventType;     
#   uint32  SpacecraftID; 
#   uint32  ProcessorID;

# Message
#   char    Message[122]; 
#   uint8   Spare1;                               
#   uint8   Spare2;         

import sys
import getopt
import zmq

from PyQt4 import QtGui, QtCore
from EventMessageDialog import Ui_EventMessageDialog
from struct import *

class EventMessageTelemetry(QtGui.QDialog):

    pktCount = 0

    def __init__(self, appId):
        QtGui.QDialog.__init__(self)
        self.appId = appId
        self.ui = Ui_EventMessageDialog()
        self.ui.setupUi(self)
    
    def initTlmReceiver(self, subscription):
        self.setWindowTitle(pageTitle + ' for: ' + subscription)
        self.thread = TlmReceiver(self, subscription, self.appId)
        self.connect(self.thread, self.thread.signalTlmDatagram, self.processPendingDatagrams)
        self.thread.start()

    # This method processes packets. Called when the TelemetryReceiver receives a message/packet
    def processPendingDatagrams(self, datagram):
        
        self.pktCount += 1

        # Packet Header
        #   uint16  StreamId;   0   
        #   uint16  Sequence;   2
        #   uint16  Length;     4 
        # PktSequence = unpack("<H",datagram[2:4])
        self.ui.sequenceCount.setText(str(self.pktCount))

        #
        # Not accounting for endian right now!
        #
        appName = "".join(unpack("<20s",datagram[12:32]))
        eventText = "".join(unpack("<122sxx",datagram[44:]))
        appName = appName.split("\0")[0]
        eventText = eventText.split("\0")[0]
        eventString = "EVENT ---> "+ appName + " : " + eventText
        self.ui.eventOutput.append(eventString)


# Subscribes and receives zeroMQ messages
class TlmReceiver(QtCore.QThread):
    
    def __init__(self, mainWindow, subscription, appId):
        QtCore.QThread.__init__(self)
        self.appId = appId

        # Setup signal to communicate with front-end GUI
        self.signalTlmDatagram = QtCore.SIGNAL("TlmDatagram")

        # Init zeroMQ
        self.context   = zmq.Context()
        self.subscriber = self.context.socket(zmq.SUB)
        self.subscriber.connect("ipc:///tmp/GroundSystem")
        self.subscriber.setsockopt(zmq.SUBSCRIBE, subscription)
    
    def run(self):
        while True:
            # Read envelope with address
            [address, datagram] = self.subscriber.recv_multipart()
            #print("[%s] %s" % (address, datagram))
            # Ignore if not an event message
            if self.appId not in address: continue
            self.emit(self.signalTlmDatagram, datagram)

#
# Display usage
#
def usage():
    print "Must specify --title=<page name> --port=<udp_port> --appid=<packet_app_id(hex)> --endian=<endian(L|B) --file=<tlm_def_file>"
    print "     example: --title=Executive Services --port=10800 --appid=800 --file=cfe-es-hk-table.txt --endian=L"
    print "            (quotes are not on the title string in this example)"

 
if __name__ == '__main__':


    #
    # Set defaults for the arguments
    #
    pageTitle = "Event Messages"
    udpPort  = 10000
    appId = 999
    tlmDefFile = "not-needed.txt"
    endian = "L"
    subscription = ""

    #
    # process cmd line args 
    #
    try:
        opts, args = getopt.getopt(sys.argv[1:], "htpafl", ["help", "title=", "port=", "appid=","file=", "endian=", "sub="])
    except getopt.GetoptError:
        usage()
        sys.exit(2)

    for opt, arg in opts:
        if opt in ("-h", "--help"):
            usage()
            sys.exit()
        elif opt in ("-p", "--port"):
            udpPort = arg
        elif opt in ("-t", "--title"):
            pageTitle = arg
        elif opt in ("-f", "--file"):
            tlmDefFile = arg
        elif opt in ("-t", "--appid"):
            appId = arg
        elif opt in ("-e", "--endian"):
            endian = arg
        elif opt in ("-s", "--sub"):
            subscription = arg

    if len(subscription) == 0:
        subscription = "GroundSystem"

    arr = subscription.split('.')
    if len(arr) < 3:
        subscription = 'GroundSystem'

    print 'Event Messages Page started. Subscribed to ' + subscription

    if endian == 'L':
       py_endian = '<'
    else:
       py_endian = '>'

    #
    # Init the QT application and the Event Message class
    #
    app = QtGui.QApplication(sys.argv)
    Telem = EventMessageTelemetry(appId)

    # Display the page
    Telem.show()
    Telem.raise_()
    Telem.initTlmReceiver(subscription)

    sys.exit(app.exec_())
