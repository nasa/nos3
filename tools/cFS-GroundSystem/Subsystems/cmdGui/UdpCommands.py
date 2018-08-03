#!/usr/bin/env python
# 
# UdpCommands.py -- This is a class that creates a simple command dialog and 
#                   sends commands using the cmdUtil UDP C program. 
#     
#                   The commands that can be sent are defined in comma delimited text files.
#                   This class deals with one set of commands in a file ( up to 20 ) 
#                   but multiple subsystems can be represented by spawning this class
#                   multiple times.
#
#                   This class could be duplicated to send over another interface
#                   such as TCP, Cubesat Space Protocol, or Xbee wireless radio
#

import sys
import csv
import getopt 
import subprocess 
import shlex
import pickle

from PyQt4 import QtGui
from GenericCommandDialog import Ui_GenericCommandDialog
from HTMLParser import HTMLParser
from HTMLDocsParser import HTMLDocsParser
from struct import *

class SubsystemCommands(QtGui.QDialog):

    pktCount = 0

    #
    # Init the class
    #
    def __init__(self):
        QtGui.QDialog.__init__(self)
        self.ui = Ui_GenericCommandDialog()
        self.ui.setupUi(self)
        self.setWindowTitle(pageTitle)
        self.parser = HTMLDocsParser()

        self.ui.SendButton_1.clicked.connect(self.ProcessSendButton_1)
        self.ui.SendButton_2.clicked.connect(self.ProcessSendButton_2)
        self.ui.SendButton_3.clicked.connect(self.ProcessSendButton_3)
        self.ui.SendButton_4.clicked.connect(self.ProcessSendButton_4)
        self.ui.SendButton_5.clicked.connect(self.ProcessSendButton_5)
        self.ui.SendButton_6.clicked.connect(self.ProcessSendButton_6)
        self.ui.SendButton_7.clicked.connect(self.ProcessSendButton_7)
        self.ui.SendButton_8.clicked.connect(self.ProcessSendButton_8)
        self.ui.SendButton_9.clicked.connect(self.ProcessSendButton_9)
        self.ui.SendButton_10.clicked.connect(self.ProcessSendButton_10)
        self.ui.SendButton_11.clicked.connect(self.ProcessSendButton_11)
        self.ui.SendButton_12.clicked.connect(self.ProcessSendButton_12)
        self.ui.SendButton_13.clicked.connect(self.ProcessSendButton_13)
        self.ui.SendButton_14.clicked.connect(self.ProcessSendButton_14)
        self.ui.SendButton_15.clicked.connect(self.ProcessSendButton_15)
        self.ui.SendButton_16.clicked.connect(self.ProcessSendButton_16)
        self.ui.SendButton_17.clicked.connect(self.ProcessSendButton_17)
        self.ui.SendButton_18.clicked.connect(self.ProcessSendButton_18)
        self.ui.SendButton_19.clicked.connect(self.ProcessSendButton_19)
        self.ui.SendButton_20.clicked.connect(self.ProcessSendButton_20)
        self.ui.SendButton_21.clicked.connect(self.ProcessSendButton_21)
        self.ui.SendButton_22.clicked.connect(self.ProcessSendButton_22)
        self.ui.SendButton_23.clicked.connect(self.ProcessSendButton_23)
        self.ui.SendButton_24.clicked.connect(self.ProcessSendButton_24)
        self.ui.SendButton_25.clicked.connect(self.ProcessSendButton_25)

    #
    # Button press methods
    #  
    def ProcessSendButton_1(self):
        self.ProcessSendButtonGeneric(0)
    def ProcessSendButton_2(self):
        self.ProcessSendButtonGeneric(1)
    def ProcessSendButton_3(self):
        self.ProcessSendButtonGeneric(2)
    def ProcessSendButton_4(self):
        self.ProcessSendButtonGeneric(3)
    def ProcessSendButton_5(self):
        self.ProcessSendButtonGeneric(4)
    def ProcessSendButton_6(self):
        self.ProcessSendButtonGeneric(5)
    def ProcessSendButton_7(self):
        self.ProcessSendButtonGeneric(6)
    def ProcessSendButton_8(self):
        self.ProcessSendButtonGeneric(7)
    def ProcessSendButton_9(self):
        self.ProcessSendButtonGeneric(8)
    def ProcessSendButton_10(self):
        self.ProcessSendButtonGeneric(9)
    def ProcessSendButton_11(self):
        self.ProcessSendButtonGeneric(10)
    def ProcessSendButton_12(self):
        self.ProcessSendButtonGeneric(11)
    def ProcessSendButton_13(self):
        self.ProcessSendButtonGeneric(12)
    def ProcessSendButton_14(self):
        self.ProcessSendButtonGeneric(13)
    def ProcessSendButton_15(self):
        self.ProcessSendButtonGeneric(14)
    def ProcessSendButton_16(self):
        self.ProcessSendButtonGeneric(15)
    def ProcessSendButton_17(self):
        self.ProcessSendButtonGeneric(16)
    def ProcessSendButton_18(self):
        self.ProcessSendButtonGeneric(17)
    def ProcessSendButton_19(self):
        self.ProcessSendButtonGeneric(18)
    def ProcessSendButton_20(self):
        self.ProcessSendButtonGeneric(19)
    def ProcessSendButton_21(self):
        self.ProcessSendButtonGeneric(20)
    def ProcessSendButton_22(self):
        self.ProcessSendButtonGeneric(21)
    def ProcessSendButton_23(self):
        self.ProcessSendButtonGeneric(22)
    def ProcessSendButton_24(self):
        self.ProcessSendButtonGeneric(23)
    def ProcessSendButton_25(self):
        self.ProcessSendButtonGeneric(24)


    #
    # Determines if command requires parameters
    #
    def checkParams(self, idx):
        pickle_file = 'ParameterFiles/' + param_files[idx]
        try:
            with open(pickle_file,'rb') as pickle_obj:
                dataTypesOrig, paramNames, paramLen, paramDesc, dataTypesNew, stringLen = pickle.load(pickle_obj)
                if len(paramNames) > 0: # if has parameters
                   return True
                else:
                   return False
        except IOError:
            return False


    #
    # Generic button press method
    #
    def ProcessSendButtonGeneric(self, idx):
        if cmdItemIsValid[idx] == True: 
            param_bool = self.checkParams(idx)
            address = str(Commands.ui.commandAddressLineEdit.text())

            
            # If parameters are required, launches Parameters page
            if param_bool == True:
                prog = 'python Parameter.py'
                launch_string = prog+' --title=\"'+pageTitle+'\" --descrip=\"'+cmdDesc[idx]+'\" --idx='+str(idx)+' --host=\"'+address+'\" --port='+str(pagePort)+' --pktid='+str(pagePktId)+' --endian='+pageEndian+' --cmdcode='+cmdCodes[idx]+' --file='+param_files[idx]

            # If parameters not required, directly calls cmdUtil to send command
            else:
                prog = '../cmdUtil/cmdUtil '
                launch_string = prog + ' --host=' + address + ' --port=' + str(pagePort) + ' --pktid=' + str(pagePktId) + ' --endian=' + pageEndian + ' --cmdcode=' + cmdCodes[idx]

            cmd_args = shlex.split(launch_string)
            subprocess.Popen(cmd_args)



#
# Display usage
#
def usage():
    print "Must specify --title=<page name> --file=<cmd_def_file> --pktid=<packet_app_id(hex)> --endian=<LE|BE> --address=<IP address> --port=<UDP port>" 
    print "     example: --title=\"Executive Services\" --file=cfe-es-cmds.txt --pktid=1806  --endian=LE --address=127.0.0.1 --port=1234" 

#
# Main 
#
if __name__ == '__main__':

    #
    # Set defaults for the arguments
    #
    pageTitle = "Command Page"               
    pagePort  = 1234
    pageAddress = "127.0.0.1"
    pagePktId = 1801 
    pageEndian = "LE"
    pageDefFile = "cfe__es__msg_8h"

    #
    # process cmd line args 
    #
    try:                                
        opts, args = getopt.getopt(sys.argv[1:], "htpfeap", ["help", "title=", "pktid=", "file=", "endian=", "address=", "port="]) 
    except getopt.GetoptError:
        usage()                          
        sys.exit(2)                     

    for opt, arg in opts:                
        if opt in ("-h", "--help"):      
            usage()                     
            sys.exit()                  
        elif opt in ("-t", "--title"): 
            pageTitle = arg
        elif opt in ("-f", "--file"): 
            pageDefFile = arg
        elif opt in ("-p", "--pktid"): 
            pagePktId = arg                   
        elif opt in ("-e", "--endian"): 
            pageEndian = arg                          
        elif opt in ("-a", "--address"): 
            pageAddress = arg               
        elif opt in ("-p", "--port"): 
            pagePort = arg

    #
    # Init the QT application and the command class
    #
    app = QtGui.QApplication(sys.argv)
    Commands = SubsystemCommands()
    Commands.ui.subSystemTextBrowser.setText(pageTitle)
    Commands.ui.packetId.display(pagePktId)
    Commands.ui.commandAddressLineEdit.setText(pageAddress)

    #
    # Reads commands from command definition file
    #
    pickle_file = 'CommandFiles/'+pageDefFile
    with open(pickle_file, 'rb') as pickle_obj:
        cmdDesc, cmdCodes, param_files = pickle.load(pickle_obj)

    cmdItemIsValid = []
    for i in range(len(cmdDesc)):
        cmdItemIsValid.append(True)
    for i in range (len(cmdDesc), 26):
        cmdItemIsValid.append(False)


    #
    # Fill the data fields on the page
    #
    if cmdItemIsValid[0] == True:
       Commands.ui.itemLabelTextBrowser_1.setText(cmdDesc[0])
    else:
       Commands.ui.itemLabelTextBrowser_1.setText("(unused)")

    if cmdItemIsValid[1] == True:
       Commands.ui.itemLabelTextBrowser_2.setText(cmdDesc[1])
    else:
       Commands.ui.itemLabelTextBrowser_2.setText("(unused)")

    if cmdItemIsValid[2] == True:
       Commands.ui.itemLabelTextBrowser_3.setText(cmdDesc[2])
    else:
       Commands.ui.itemLabelTextBrowser_3.setText("(unused)")

    if cmdItemIsValid[3] == True:
       Commands.ui.itemLabelTextBrowser_4.setText(cmdDesc[3])
    else:
       Commands.ui.itemLabelTextBrowser_4.setText("(unused)")

    if cmdItemIsValid[4] == True:
       Commands.ui.itemLabelTextBrowser_5.setText(cmdDesc[4])
    else:
       Commands.ui.itemLabelTextBrowser_5.setText("(unused)")

    if cmdItemIsValid[5] == True:
       Commands.ui.itemLabelTextBrowser_6.setText(cmdDesc[5])
    else:
       Commands.ui.itemLabelTextBrowser_6.setText("(unused)")

    if cmdItemIsValid[6] == True:
       Commands.ui.itemLabelTextBrowser_7.setText(cmdDesc[6])
    else:
       Commands.ui.itemLabelTextBrowser_7.setText("(unused)")

    if cmdItemIsValid[7] == True:
       Commands.ui.itemLabelTextBrowser_8.setText(cmdDesc[7])
    else:
       Commands.ui.itemLabelTextBrowser_8.setText("(unused)")

    if cmdItemIsValid[8] == True:
       Commands.ui.itemLabelTextBrowser_9.setText(cmdDesc[8])
    else:
       Commands.ui.itemLabelTextBrowser_9.setText("(unused)")

    if cmdItemIsValid[9] == True:
       Commands.ui.itemLabelTextBrowser_10.setText(cmdDesc[9])
    else:
       Commands.ui.itemLabelTextBrowser_10.setText("(unused)")

    if cmdItemIsValid[10] == True:
       Commands.ui.itemLabelTextBrowser_11.setText(cmdDesc[10])
    else:
       Commands.ui.itemLabelTextBrowser_11.setText("(unused)")

    if cmdItemIsValid[11] == True:
       Commands.ui.itemLabelTextBrowser_12.setText(cmdDesc[11])
    else:
       Commands.ui.itemLabelTextBrowser_12.setText("(unused)")

    if cmdItemIsValid[12] == True:
       Commands.ui.itemLabelTextBrowser_13.setText(cmdDesc[12])
    else:
       Commands.ui.itemLabelTextBrowser_13.setText("(unused)")

    if cmdItemIsValid[13] == True:
       Commands.ui.itemLabelTextBrowser_14.setText(cmdDesc[13])
    else:
       Commands.ui.itemLabelTextBrowser_14.setText("(unused)")

    if cmdItemIsValid[14] == True:
       Commands.ui.itemLabelTextBrowser_15.setText(cmdDesc[14])
    else:
       Commands.ui.itemLabelTextBrowser_15.setText("(unused)")

    if cmdItemIsValid[15] == True:
       Commands.ui.itemLabelTextBrowser_16.setText(cmdDesc[15])
    else:
       Commands.ui.itemLabelTextBrowser_16.setText("(unused)")

    if cmdItemIsValid[16] == True:
       Commands.ui.itemLabelTextBrowser_17.setText(cmdDesc[16])
    else:
       Commands.ui.itemLabelTextBrowser_17.setText("(unused)")

    if cmdItemIsValid[17] == True:
       Commands.ui.itemLabelTextBrowser_18.setText(cmdDesc[17])
    else:
       Commands.ui.itemLabelTextBrowser_18.setText("(unused)")

    if cmdItemIsValid[18] == True:
       Commands.ui.itemLabelTextBrowser_19.setText(cmdDesc[18])
    else:
       Commands.ui.itemLabelTextBrowser_19.setText("(unused)")

    if cmdItemIsValid[19] == True:
       Commands.ui.itemLabelTextBrowser_20.setText(cmdDesc[19])
    else:
       Commands.ui.itemLabelTextBrowser_20.setText("(unused)")

    if cmdItemIsValid[20] == True:
       Commands.ui.itemLabelTextBrowser_21.setText(cmdDesc[20])
    else:
       Commands.ui.itemLabelTextBrowser_21.setText("(unused)")

    if cmdItemIsValid[21] == True:
       Commands.ui.itemLabelTextBrowser_22.setText(cmdDesc[21])
    else:
       Commands.ui.itemLabelTextBrowser_22.setText("(unused)")

    if cmdItemIsValid[22] == True:
       Commands.ui.itemLabelTextBrowser_23.setText(cmdDesc[22])
    else:
       Commands.ui.itemLabelTextBrowser_23.setText("(unused)")

    if cmdItemIsValid[23] == True:
       Commands.ui.itemLabelTextBrowser_24.setText(cmdDesc[23])
    else:
       Commands.ui.itemLabelTextBrowser_24.setText("(unused)")

    if cmdItemIsValid[24] == True:
       Commands.ui.itemLabelTextBrowser_25.setText(cmdDesc[24])
    else:
       Commands.ui.itemLabelTextBrowser_25.setText("(unused)")


    # 
    # Display the page 
    #
    Commands.show()
    Commands.raise_()
    sys.exit(app.exec_())
