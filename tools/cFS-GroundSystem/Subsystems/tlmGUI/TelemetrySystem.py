#!/usr/bin/env python
# 
import sys
import csv
import subprocess
import getopt
import shlex
import zmq

from PyQt4 import QtGui, QtCore
from TelemetrySystemDialog import Ui_TelemetrySystemDialog
from struct import *

class TelemetrySystem(QtGui.QDialog):

    pktCount = 0

    #
    # Init the class
    #
    def __init__(self):
        QtGui.QDialog.__init__(self)
        self.ui = Ui_TelemetrySystemDialog()
        self.ui.setupUi(self)
        self.setWindowTitle('Telemetry System Main Page')
        self.move(0,100)

        self.ui.pushButton_0.clicked.connect(self.ProcessButton_0)
        self.ui.pushButton_1.clicked.connect(self.ProcessButton_1)
        self.ui.pushButton_2.clicked.connect(self.ProcessButton_2)
        self.ui.pushButton_3.clicked.connect(self.ProcessButton_3)
        self.ui.pushButton_4.clicked.connect(self.ProcessButton_4)
        self.ui.pushButton_5.clicked.connect(self.ProcessButton_5)
        self.ui.pushButton_6.clicked.connect(self.ProcessButton_6)
        self.ui.pushButton_7.clicked.connect(self.ProcessButton_7)
        self.ui.pushButton_8.clicked.connect(self.ProcessButton_8)
        self.ui.pushButton_9.clicked.connect(self.ProcessButton_9)
        self.ui.pushButton_10.clicked.connect(self.ProcessButton_10)
        self.ui.pushButton_11.clicked.connect(self.ProcessButton_11)
        self.ui.pushButton_12.clicked.connect(self.ProcessButton_12)
        self.ui.pushButton_13.clicked.connect(self.ProcessButton_13)
        self.ui.pushButton_14.clicked.connect(self.ProcessButton_14)
        self.ui.pushButton_15.clicked.connect(self.ProcessButton_15)
        self.ui.pushButton_16.clicked.connect(self.ProcessButton_16)
        self.ui.pushButton_17.clicked.connect(self.ProcessButton_17)
        self.ui.pushButton_18.clicked.connect(self.ProcessButton_18)
        self.ui.pushButton_19.clicked.connect(self.ProcessButton_19)
        self.ui.pushButton_20.clicked.connect(self.ProcessButton_20)
    #
    # convert a string of binary bytes to ascii hex 
    #
    def strToHex(aString):
        hexStr = ""
        for x in aString:
            hexStr = hexStr + "%02X " % ord(x)
        return hexStr

    #
    # Dump the telemetry packet 
    #
    def dumpPacket(packetData):
         appIdString = "%02X" % ord(packetData[0])
         appIdString = appIdString + "%02X" % ord(packetData[1])
         appId = (ord(packetData[0]) << 8) + (ord(packetData[1]))
         print "\n-----------------------------------------------"
         print "\nPacket: App ID = ",  hex(appId)
         print "\nPacket Data: ", strToHex(packetData)

    #
    # Button press methods
    #  
    def ProcessButton_0(self):
        self.ProcessButtonGeneric(0)
    def ProcessButton_1(self):
        self.ProcessButtonGeneric(1)
    def ProcessButton_2(self):
        self.ProcessButtonGeneric(2)
    def ProcessButton_3(self):
        self.ProcessButtonGeneric(3)
    def ProcessButton_4(self):
        self.ProcessButtonGeneric(4)
    def ProcessButton_5(self):
        self.ProcessButtonGeneric(5)
    def ProcessButton_6(self):
        self.ProcessButtonGeneric(6)
    def ProcessButton_7(self):
        self.ProcessButtonGeneric(7)
    def ProcessButton_8(self):
        self.ProcessButtonGeneric(8)
    def ProcessButton_9(self):
        self.ProcessButtonGeneric(9)
    def ProcessButton_10(self):
        self.ProcessButtonGeneric(10)
    def ProcessButton_11(self):
        self.ProcessButtonGeneric(11)
    def ProcessButton_12(self):
        self.ProcessButtonGeneric(12)
    def ProcessButton_13(self):
        self.ProcessButtonGeneric(13)
    def ProcessButton_14(self):
        self.ProcessButtonGeneric(14)
    def ProcessButton_15(self):
        self.ProcessButtonGeneric(15)
    def ProcessButton_16(self):
        self.ProcessButtonGeneric(16)
    def ProcessButton_17(self):
        self.ProcessButtonGeneric(17)
    def ProcessButton_18(self):
        self.ProcessButtonGeneric(18)
    def ProcessButton_19(self):
        self.ProcessButtonGeneric(19)
    def ProcessButton_20(self):
        self.ProcessButtonGeneric(20)

    def ProcessButtonGeneric(self, idx):
        tempSub = self.subscription + '.' + hex(tlmPageAppid[idx])
        if tlmPageIsValid[idx] == True:
           # need to extract data from fields, then start page with right params
           launch_string = 'python ' + tlmClass[idx] + ' --title=\"' + tlmPageDesc[idx] + '\" --appid=' + hex(tlmPageAppid[idx]) + ' --port=' + str(tlmPagePort[idx]) + ' --file=' + tlmPageDefFile[idx] + ' --endian=' + endian + ' --sub=' + tempSub
           cmd_args = shlex.split(launch_string)
           subprocess.Popen(cmd_args)
    
    # Start the telemetry receiver (see TlmReceiver class)
    def initTlmReceiver(self, subscription):
        self.setWindowTitle('Telemetry System page for: ' + subscription)
        self.subscription = subscription
        self.thread = TlmReceiver(self, subscription)
        self.connect(self.thread, self.thread.signalTlmDatagram, self.processPendingDatagrams)
        self.thread.start()

    #
    # This method processes packets. Called when the TelemetryReceiver receives a message/packet
    #
    def processPendingDatagrams(self, datagram):
        
        #
        # convert a string of binary bytes to ascii hex 
        #
        def strToHex(aString):
           hexStr = ""
           for x in aString:
               hexStr = hexStr + "%02X " % ord(x)
           return hexStr

        #
        # Dump the telemetry packet 
        #
        def dumpPacket(packetData):
            appIdString = "%02X" % ord(packetData[0])
            appIdString = appIdString + "%02X" % ord(packetData[1])
            appId = (ord(packetData[0]) << 8) + (ord(packetData[1]))
            print appIdString
            print "\nPacket: App ID = ",  hex(appId)
            print "\nPacket Data: ", strToHex(packetData)
            print "\n-----------------------------------------------"

        #
        # Show number of packets received
        # 
        self.pktCount += 1
        self.ui.sequenceCount.setText(str(self.pktCount))

        #
        # Decode the packet and forward it to the correct port (if there is one) 
        #
        streamId,Sequence,Length = unpack(">HHH",datagram[:6])

        # Uncomment the next two lines to debug
        # print "Packet ID = " , hex(streamId)
        # dumpPacket(datagram)
        for i in range ( 0, 21 ):
           if streamId == tlmPageAppid[i]:
               send_host = "127.0.0.1"
               send_port =  tlmPagePort[i] 
#               sendSocket = socket(AF_INET,SOCK_DGRAM)
#               sendSocket.sendto(datagram, (send_host,send_port))

               tlmPageCount[i] = tlmPageCount[i] + 1
               #
               # I wish I knew a better way to update the count field in the GUI
               # Maybe store a pointer to the field in the gui
               #
               if i == 0: 
                  self.ui.countBrowser_0.setText(str(tlmPageCount[0]))
               elif i == 1: 
                 self.ui.countBrowser_1.setText(str(tlmPageCount[1]))
               elif i == 2: 
                 self.ui.countBrowser_2.setText(str(tlmPageCount[2]))
               elif i == 3: 
                 self.ui.countBrowser_3.setText(str(tlmPageCount[3]))
               elif i == 4: 
                 self.ui.countBrowser_4.setText(str(tlmPageCount[4]))
               elif i == 5: 
                 self.ui.countBrowser_5.setText(str(tlmPageCount[5]))
               elif i == 6: 
                 self.ui.countBrowser_6.setText(str(tlmPageCount[6]))
               elif i == 7: 
                 self.ui.countBrowser_7.setText(str(tlmPageCount[7]))
               elif i == 8: 
                 self.ui.countBrowser_8.setText(str(tlmPageCount[8]))
               elif i == 9: 
                 self.ui.countBrowser_9.setText(str(tlmPageCount[9]))
               elif i == 10: 
                 self.ui.countBrowser_10.setText(str(tlmPageCount[10]))
               elif i == 11: 
                 self.ui.countBrowser_11.setText(str(tlmPageCount[11]))
               elif i == 12: 
                 self.ui.countBrowser_12.setText(str(tlmPageCount[12]))
               elif i == 13: 
                 self.ui.countBrowser_13.setText(str(tlmPageCount[13]))
               elif i == 14: 
                 self.ui.countBrowser_14.setText(str(tlmPageCount[14]))
               elif i == 15: 
                 self.ui.countBrowser_16.setText(str(tlmPageCount[15]))
               elif i == 16: 
                 self.ui.countBrowser_17.setText(str(tlmPageCount[16]))
               elif i == 17: 
                 self.ui.countBrowser_18.setText(str(tlmPageCount[17]))
               elif i == 18: 
                 self.ui.countBrowser_19.setText(str(tlmPageCount[18]))
               elif i == 19: 
                 self.ui.countBrowser_20.setText(str(tlmPageCount[19]))
               elif i == 20: 
                 self.ui.countBrowser_21.setText(str(tlmPageCount[20]))


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
            # Receive and read envelope with address
            [address, datagram] = self.subscriber.recv_multipart()
            # Send signal with received packet to front-end/GUI
            self.emit(self.signalTlmDatagram, datagram)

#
# Main
#
if __name__ == '__main__':


    #
    # Init the QT application and the telemetry dialog class
    #
    app = QtGui.QApplication(sys.argv)
    Telem = TelemetrySystem()


    #
    # Set defaults for the arguments
    #
    tlmDefFile = "telemetry-pages.txt"
    endian = "L"
    subscription = ""
    
    #
    # process cmd line args
    #
    try:
        opts, args = getopt.getopt(sys.argv[1:], "htpafl", ["sub="])
    except getopt.GetoptError:
        sys.exit(2)
        
    for opt, arg in opts:
        if opt in ("-s", "--sub"):
            subscription = arg

    if len(subscription) == 0:
        subscription = "GroundSystem"

    print 'Telemetry System started. Subscribed to ' + subscription
    #
    # Read in the contents of the telemetry packet defintion
    #
    tlmPageIsValid = []
    tlmPageDesc = []
    tlmClass = []
    tlmPagePort = []
    tlmPageAppid = []
    tlmPageCount = []
    tlmPageDefFile = []
    i = 0

    with open(tlmDefFile, 'rb') as tlmfile:
       reader = csv.reader(tlmfile, skipinitialspace = True)
       for row in reader:
          if row[0][0] != '#':
             tlmPageIsValid.append(True)
             tlmPageDesc.append(row[0])
             tlmClass.append(row[1])
             tlmPagePort.append(int(row[2],16) + 10000)
             tlmPageAppid.append(int(row[2],16))
             tlmPageDefFile.append(row[3])
             tlmPageCount.append(0)
             i = i + 1
    # 
    # Mark the remaining values ad invalid
    #
    for j in range (i, 21):
       tlmPageAppid.append(0)
       tlmPageIsValid.append(False)

    #
    # fill the data fields on the page
    #
    if tlmPageIsValid[0] == True:
       Telem.ui.SubsysBrowser_0.setText(tlmPageDesc[0])
       Telem.ui.pktidBrowser_0.setText(hex(tlmPageAppid[0]))
       Telem.ui.countBrowser_0.setText(str(tlmPageCount[0]))
    else:
       Telem.ui.SubsysBrowser_0.setText("(unused)")

    if tlmPageIsValid[1] == True:
       Telem.ui.SubsysBrowser_1.setText(tlmPageDesc[1])
       Telem.ui.pktidBrowser_1.setText(hex(tlmPageAppid[1]))
       Telem.ui.countBrowser_1.setText(str(tlmPageCount[1]))
    else:
       Telem.ui.SubsysBrowser_1.setText("(unused)")

    if tlmPageIsValid[2] == True:
       Telem.ui.SubsysBrowser_2.setText(tlmPageDesc[2])
       Telem.ui.pktidBrowser_2.setText(hex(tlmPageAppid[2]))
       Telem.ui.countBrowser_2.setText(str(tlmPageCount[2]))
    else:
       Telem.ui.SubsysBrowser_2.setText("(unused)")

    if tlmPageIsValid[3] == True:
       Telem.ui.SubsysBrowser_3.setText(tlmPageDesc[3])
       Telem.ui.pktidBrowser_3.setText(hex(tlmPageAppid[3]))
       Telem.ui.countBrowser_3.setText(str(tlmPageCount[3]))
    else:
       Telem.ui.SubsysBrowser_3.setText("(unused)")

    if tlmPageIsValid[4] == True:
       Telem.ui.SubsysBrowser_4.setText(tlmPageDesc[4])
       Telem.ui.pktidBrowser_4.setText(hex(tlmPageAppid[4]))
       Telem.ui.countBrowser_4.setText(str(tlmPageCount[4]))
    else:
       Telem.ui.SubsysBrowser_4.setText("(unused)")

    if tlmPageIsValid[5] == True:
       Telem.ui.SubsysBrowser_5.setText(tlmPageDesc[5])
       Telem.ui.pktidBrowser_5.setText(hex(tlmPageAppid[5]))
       Telem.ui.countBrowser_5.setText(str(tlmPageCount[5]))
    else:
       Telem.ui.SubsysBrowser_5.setText("(unused)")

    if tlmPageIsValid[6] == True:
       Telem.ui.SubsysBrowser_6.setText(tlmPageDesc[6])
       Telem.ui.pktidBrowser_6.setText(hex(tlmPageAppid[6]))
       Telem.ui.countBrowser_6.setText(str(tlmPageCount[6]))
    else:
       Telem.ui.SubsysBrowser_6.setText("(unused)")

    if tlmPageIsValid[7] == True:
       Telem.ui.SubsysBrowser_7.setText(tlmPageDesc[7])
       Telem.ui.pktidBrowser_7.setText(hex(tlmPageAppid[7]))
       Telem.ui.countBrowser_7.setText(str(tlmPageCount[7]))
    else:
       Telem.ui.SubsysBrowser_7.setText("(unused)")

    if tlmPageIsValid[8] == True:
       Telem.ui.SubsysBrowser_8.setText(tlmPageDesc[8])
       Telem.ui.pktidBrowser_8.setText(hex(tlmPageAppid[8]))
       Telem.ui.countBrowser_8.setText(str(tlmPageCount[8]))
    else:
       Telem.ui.SubsysBrowser_8.setText("(unused)")

    if tlmPageIsValid[9] == True:
       Telem.ui.SubsysBrowser_9.setText(tlmPageDesc[9])
       Telem.ui.pktidBrowser_9.setText(hex(tlmPageAppid[9]))
       Telem.ui.countBrowser_9.setText(str(tlmPageCount[9]))
    else:
       Telem.ui.SubsysBrowser_9.setText("(unused)")

    if tlmPageIsValid[10] == True:
       Telem.ui.SubsysBrowser_10.setText(tlmPageDesc[10])
       Telem.ui.pktidBrowser_10.setText(hex(tlmPageAppid[10]))
       Telem.ui.countBrowser_10.setText(str(tlmPageCount[10]))
    else:
       Telem.ui.SubsysBrowser_10.setText("(unused)")

    if tlmPageIsValid[11] == True:
       Telem.ui.SubsysBrowser_11.setText(tlmPageDesc[11])
       Telem.ui.pktidBrowser_11.setText(hex(tlmPageAppid[11]))
       Telem.ui.countBrowser_11.setText(str(tlmPageCount[11]))
    else:
       Telem.ui.SubsysBrowser_11.setText("(unused)")

    if tlmPageIsValid[12] == True:
       Telem.ui.SubsysBrowser_12.setText(tlmPageDesc[12])
       Telem.ui.pktidBrowser_12.setText(hex(tlmPageAppid[12]))
       Telem.ui.countBrowser_12.setText(str(tlmPageCount[12]))
    else:
       Telem.ui.SubsysBrowser_12.setText("(unused)")

    if tlmPageIsValid[13] == True:
       Telem.ui.SubsysBrowser_13.setText(tlmPageDesc[13])
       Telem.ui.pktidBrowser_13.setText(hex(tlmPageAppid[13]))
       Telem.ui.countBrowser_13.setText(str(tlmPageCount[13]))
    else:
       Telem.ui.SubsysBrowser_13.setText("(unused)")

    if tlmPageIsValid[14] == True:
       Telem.ui.SubsysBrowser_14.setText(tlmPageDesc[14])
       Telem.ui.pktidBrowser_14.setText(hex(tlmPageAppid[14]))
       Telem.ui.countBrowser_14.setText(str(tlmPageCount[14]))
    else:
       Telem.ui.SubsysBrowser_14.setText("(unused)")

    if tlmPageIsValid[15] == True:
       Telem.ui.SubsysBrowser_15.setText(tlmPageDesc[15])
       Telem.ui.pktidBrowser_15.setText(hex(tlmPageAppid[15]))
       Telem.ui.countBrowser_15.setText(str(tlmPageCount[15]))
    else:
       Telem.ui.SubsysBrowser_15.setText("(unused)")

    if tlmPageIsValid[16] == True:
       Telem.ui.SubsysBrowser_16.setText(tlmPageDesc[16])
       Telem.ui.pktidBrowser_16.setText(hex(tlmPageAppid[16]))
       Telem.ui.countBrowser_16.setText(str(tlmPageCount[16]))
    else:
       Telem.ui.SubsysBrowser_16.setText("(unused)")

    if tlmPageIsValid[17] == True:
       Telem.ui.SubsysBrowser_17.setText(tlmPageDesc[17])
       Telem.ui.pktidBrowser_17.setText(hex(tlmPageAppid[17]))
       Telem.ui.countBrowser_17.setText(str(tlmPageCount[17]))
    else:
       Telem.ui.SubsysBrowser_17.setText("(unused)")

    if tlmPageIsValid[18] == True:
       Telem.ui.SubsysBrowser_18.setText(tlmPageDesc[18])
       Telem.ui.pktidBrowser_18.setText(hex(tlmPageAppid[18]))
       Telem.ui.countBrowser_18.setText(str(tlmPageCount[18]))
    else:
       Telem.ui.SubsysBrowser_18.setText("(unused)")

    if tlmPageIsValid[19] == True:
       Telem.ui.SubsysBrowser_19.setText(tlmPageDesc[19])
       Telem.ui.pktidBrowser_19.setText(hex(tlmPageAppid[19]))
       Telem.ui.countBrowser_19.setText(str(tlmPageCount[19]))
    else:
       Telem.ui.SubsysBrowser_19.setText("(unused)")

    if tlmPageIsValid[20] == True:
       Telem.ui.SubsysBrowser_20.setText(tlmPageDesc[20])
       Telem.ui.pktidBrowser_20.setText(hex(tlmPageAppid[20]))
       Telem.ui.countBrowser_20.setText(str(tlmPageCount[20]))
    else:
       Telem.ui.SubsysBrowser_20.setText("(unused)")



    # 
    # Display the page 
    #
    Telem.show()
    Telem.raise_()
    Telem.initTlmReceiver(subscription)
    sys.exit(app.exec_())

