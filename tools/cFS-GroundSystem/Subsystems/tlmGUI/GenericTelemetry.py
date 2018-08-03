#!/usr/bin/env python
# 

import sys
import csv
import getopt 
import pdb
import zmq

from PyQt4 import QtGui, QtCore
from GenericTelemetryDialog import Ui_GenericTelemetryDialog
from struct import *

class SubsystemTelemetry(QtGui.QDialog):

    pktCount = 0

    #
    # Init the class
    #
    def __init__(self):
        QtGui.QDialog.__init__(self)
        self.ui = Ui_GenericTelemetryDialog()
        self.ui.setupUi(self)

    # 
    # This method Decodes a telemetry item from the packet and displays it 
    #
    def displayTelemetryItem(self, datagram, tlmIndex, labelField, valueField):
       if tlmItemIsValid[tlmIndex] == True:
          TlmField =   unpack(tlmItemFormat[tlmIndex], datagram[int(tlmItemStart[tlmIndex]):(int(tlmItemStart[tlmIndex]) + int(tlmItemSize[tlmIndex]))])
          if tlmItemDisplayType[tlmIndex] == 'Dec':
             valueField.setText(str(TlmField[0]))
          elif tlmItemDisplayType[tlmIndex] == 'Hex':
             valueField.setText(hex(TlmField[0]))
          elif tlmItemDisplayType[tlmIndex] == 'Enm':
             valueField.setText(tlmItemEnum[tlmIndex][int(TlmField[0])])
          elif tlmItemDisplayType[tlmIndex] == 'Str':
             valueField.setText(TlmField[0])
          labelField.setText(tlmItemDesc[tlmIndex])
       else:
          labelField.setText("(unused)")

    # Start the telemetry receiver (see TlmReceiver class)
    def initTlmReceiver(self, subscription):
        self.setWindowTitle(pageTitle + ' for: ' + subscription)
        self.thread = TlmReceiver(self, subscription)
        self.connect(self.thread, self.thread.signalTlmDatagram, self.processPendingDatagrams)
        self.thread.start()

    #
    # This method processes packets. Called when the TelemetryReceiver receives a message/packet
    #
    def processPendingDatagrams(self, datagram):
        #
        # Show number of packets received
        #
        self.pktCount += 1
        self.ui.sequenceCount.setText(str(self.pktCount))

        #
        # Decode and display all packet elements
        #
        self.displayTelemetryItem(datagram, 0, self.ui.itemLabelTextBrowser_1, self.ui.itemValueTextBrowser_1)
        self.displayTelemetryItem(datagram, 1, self.ui.itemLabelTextBrowser_2, self.ui.itemValueTextBrowser_2)
        self.displayTelemetryItem(datagram, 2, self.ui.itemLabelTextBrowser_3, self.ui.itemValueTextBrowser_3)
        self.displayTelemetryItem(datagram, 3, self.ui.itemLabelTextBrowser_4, self.ui.itemValueTextBrowser_4)
        self.displayTelemetryItem(datagram, 4, self.ui.itemLabelTextBrowser_5, self.ui.itemValueTextBrowser_5)
        self.displayTelemetryItem(datagram, 5, self.ui.itemLabelTextBrowser_6, self.ui.itemValueTextBrowser_6)
        self.displayTelemetryItem(datagram, 6, self.ui.itemLabelTextBrowser_7, self.ui.itemValueTextBrowser_7)
        self.displayTelemetryItem(datagram, 7, self.ui.itemLabelTextBrowser_8, self.ui.itemValueTextBrowser_8)
        self.displayTelemetryItem(datagram, 8, self.ui.itemLabelTextBrowser_9, self.ui.itemValueTextBrowser_9)
        self.displayTelemetryItem(datagram, 9, self.ui.itemLabelTextBrowser_10, self.ui.itemValueTextBrowser_10)
        self.displayTelemetryItem(datagram, 10, self.ui.itemLabelTextBrowser_11, self.ui.itemValueTextBrowser_11)
        self.displayTelemetryItem(datagram, 11, self.ui.itemLabelTextBrowser_12, self.ui.itemValueTextBrowser_12)
        self.displayTelemetryItem(datagram, 12, self.ui.itemLabelTextBrowser_13, self.ui.itemValueTextBrowser_13)
        self.displayTelemetryItem(datagram, 13, self.ui.itemLabelTextBrowser_14, self.ui.itemValueTextBrowser_14)
        self.displayTelemetryItem(datagram, 14, self.ui.itemLabelTextBrowser_15, self.ui.itemValueTextBrowser_15)
        self.displayTelemetryItem(datagram, 15, self.ui.itemLabelTextBrowser_16, self.ui.itemValueTextBrowser_16)
        self.displayTelemetryItem(datagram, 16, self.ui.itemLabelTextBrowser_17, self.ui.itemValueTextBrowser_17)
        self.displayTelemetryItem(datagram, 17, self.ui.itemLabelTextBrowser_18, self.ui.itemValueTextBrowser_18)
        self.displayTelemetryItem(datagram, 18, self.ui.itemLabelTextBrowser_19, self.ui.itemValueTextBrowser_19)
        self.displayTelemetryItem(datagram, 19, self.ui.itemLabelTextBrowser_20, self.ui.itemValueTextBrowser_20)
        self.displayTelemetryItem(datagram, 20, self.ui.itemLabelTextBrowser_21, self.ui.itemValueTextBrowser_21)
        self.displayTelemetryItem(datagram, 21, self.ui.itemLabelTextBrowser_22, self.ui.itemValueTextBrowser_22)
        self.displayTelemetryItem(datagram, 22, self.ui.itemLabelTextBrowser_23, self.ui.itemValueTextBrowser_23)
        self.displayTelemetryItem(datagram, 23, self.ui.itemLabelTextBrowser_24, self.ui.itemValueTextBrowser_24)
        self.displayTelemetryItem(datagram, 24, self.ui.itemLabelTextBrowser_25, self.ui.itemValueTextBrowser_25)
        self.displayTelemetryItem(datagram, 25, self.ui.itemLabelTextBrowser_26, self.ui.itemValueTextBrowser_26)
        self.displayTelemetryItem(datagram, 26, self.ui.itemLabelTextBrowser_27, self.ui.itemValueTextBrowser_27)
        self.displayTelemetryItem(datagram, 27, self.ui.itemLabelTextBrowser_28, self.ui.itemValueTextBrowser_28)
        self.displayTelemetryItem(datagram, 28, self.ui.itemLabelTextBrowser_29, self.ui.itemValueTextBrowser_29)
        self.displayTelemetryItem(datagram, 29, self.ui.itemLabelTextBrowser_30, self.ui.itemValueTextBrowser_30)
        self.displayTelemetryItem(datagram, 30, self.ui.itemLabelTextBrowser_31, self.ui.itemValueTextBrowser_31)
        self.displayTelemetryItem(datagram, 31, self.ui.itemLabelTextBrowser_32, self.ui.itemValueTextBrowser_32)
        self.displayTelemetryItem(datagram, 32, self.ui.itemLabelTextBrowser_33, self.ui.itemValueTextBrowser_33)
        self.displayTelemetryItem(datagram, 33, self.ui.itemLabelTextBrowser_34, self.ui.itemValueTextBrowser_34)
        self.displayTelemetryItem(datagram, 34, self.ui.itemLabelTextBrowser_35, self.ui.itemValueTextBrowser_35)
        self.displayTelemetryItem(datagram, 35, self.ui.itemLabelTextBrowser_36, self.ui.itemValueTextBrowser_36)
        self.displayTelemetryItem(datagram, 36, self.ui.itemLabelTextBrowser_37, self.ui.itemValueTextBrowser_37)
        self.displayTelemetryItem(datagram, 37, self.ui.itemLabelTextBrowser_38, self.ui.itemValueTextBrowser_38)
        self.displayTelemetryItem(datagram, 38, self.ui.itemLabelTextBrowser_39, self.ui.itemValueTextBrowser_39)
        self.displayTelemetryItem(datagram, 39, self.ui.itemLabelTextBrowser_40, self.ui.itemValueTextBrowser_40)


# Subscribes and receives zeroMQ messages
class TlmReceiver(QtCore.QThread):
    
    def __init__(self, mainWindow, subscription):
        QtCore.QThread.__init__(self)

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
            # Send signal with received packet to front-end/GUI
            self.emit(self.signalTlmDatagram, datagram)


#
# Display usage
#
def usage():
    print "Must specify --title=<page name> --port=<udp_port> --appid=<packet_app_id(hex)> --endian=<endian(L|B) --file=<tlm_def_file> --sub=<subscriber_string>"
    print "     example: --title=Executive Services --port=10800 --appid=800 --file=cfe-es-hk-table.txt --endian=L --sub=GroundSystem.Spacecraft1.0x886"
    print "            (quotes are not on the title string in this example)" 

#
# Main 
#
if __name__ == '__main__':

    #
    # Set defaults for the arguments
    #
    pageTitle = "Telemetry Page"               
    udpPort  = 10000
    appId = 999 
    tlmDefFile = "telemetry_def.txt"
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

    print 'Generic Telemetry Page started. Subscribed to ' + subscription

    if endian == 'L':
       py_endian = '<'
    else:
       py_endian = '>'

    #
    # Init the QT application and the telemetry class
    #
    app = QtGui.QApplication(sys.argv)
    Telem = SubsystemTelemetry()
    Telem.ui.subSystemTextBrowser.setText(pageTitle)
    Telem.ui.packetId.display(appId)

    #
    # Read in the contents of the telemetry packet defintion
    #
    tlmItemIsValid = []
    tlmItemDesc = []
    tlmItemStart = []
    tlmItemSize = []
    tlmItemDisplayType = []
    tlmItemFormat = []
    tlmItemEnum = [[] for i in range(40)]
    i = 0

    with open(tlmDefFile, 'rb') as tlmfile:
       reader = csv.reader(tlmfile, skipinitialspace = True)
       for row in reader:
          if row[0][0] != '#':
             tlmItemIsValid.append(True)
             tlmItemDesc.append(row[0])
             tlmItemStart.append(row[1])
             tlmItemSize.append(row[2])
             if row[3] == 's':
                tlmItemFormat.append(row[2]+row[3])
             else:
                tlmItemFormat.append(py_endian + row[3])

             tlmItemDisplayType.append(row[4])
             if row[4] == 'Enm':
                tlmItemEnum[i].append(row[5])
                tlmItemEnum[i].append(row[6])
                tlmItemEnum[i].append(row[7])
                tlmItemEnum[i].append(row[8])
             i = i + 1

    # 
    # Mark the remaining values as invalid
    #
    for j in range (i, 40):
       tlmItemIsValid.append(False)

    # 
    # Display the page 
    #
    Telem.show()
    Telem.raise_()
    Telem.initTlmReceiver(subscription)

    sys.exit(app.exec_())

