#cFS Ground System Version 2.0.0
#
#!/usr/bin/env python
#
import sys
import os
import socket
import zmq

from PyQt4 import QtGui, QtNetwork, QtCore
from MainWindow import Ui_MainWindow
from RoutingService import RoutingService

#
# CFS Ground System: Setup and manage the main window
#
class GroundSystem(QtGui.QMainWindow):

    #
    # Init the class
    #
    def __init__(self, parent=None):
        QtGui.QMainWindow.__init__(self)

        # Init lists
        self.ipAddressesList = ['All']
        self.spacecraftNames = ['All']

        # Init GUI and set callback methods for buttons
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.ui.pushButtonStartTlm.clicked.connect(self.startTlmSystem)
        self.ui.pushButtonStartCmd.clicked.connect(self.startCmdSystem)

    def closeEvent(self, evnt):
        if self.RoutingService:
            self.RoutingService.stop()
            print "Stopped routing service"

        super(GroundSystem, self).closeEvent(evnt)

    # Read the selected spacecraft from combo box on GUI
    def getSelectedSpacecraftAddress(self):
        return str(self.ui.comboBoxIpAddresses.currentText())

    # Returns the name of the selected spacecraft
    def getSelectedSpacecraftName(self):
        return self.spacecraftNames[self.ipAddressesList.index(self.getSelectedSpacecraftAddress())]

    #
    # Display popup with error
    #
    def DisplayErrorMessage(self, message):
        print message
        alert = QtGui.QMessageBox()
        alert.setText(message)
        alert.setIcon(QtGui.QMessageBox.Warning)
        alert.exec_()

    # Start the telemetry system for the selected spacecraft
    def startTlmSystem(self):
        selectedSpacecraft = self.getSelectedSpacecraftName()

        # Setup the subscription (to let know the telemetry system the messages it will be receiving)
        if selectedSpacecraft == 'All':
            subscription = '--sub=GroundSystem'
        else:
            subscription = '--sub=GroundSystem.' + selectedSpacecraft + '.TelemetryPackets'

        # Open Telemetry System
        system_call = '( cd Subsystems/tlmGUI/ && python TelemetrySystem.py ' + subscription + ' ) & '
        os.system(system_call)

    # Start command system
    def startCmdSystem(self):
        os.system('( cd Subsystems/cmdGui/ && python CommandSystem.py ) & ')

    # Start FDL-FUL gui system
    #def startFDLSystem(self):
    #    selectedSpacecraft = self.getSelectedSpacecraftName()
    #    if selectedSpacecraft == 'All':
    #        subscription = ''
    #        self.DisplayErrorMessage('Cannot open FDL manager.\nNo spacecraft selected.')
    #    else:
    #       subscription = '--sub=GroundSystem.' + selectedSpacecraft
    #       os.system('( cd Subsystems/fdlGui/ && python FdlSystem.py ' + subscription + ' ) & ')

    # Update the combo box list in gui
    def updateIpList(self, ip, name):
        self.ipAddressesList.append(ip)
        self.spacecraftNames.append(name)
        self.ui.comboBoxIpAddresses.addItem(ip)

    # Start the routing service (see RoutingService.py)
    def initRoutingService(self):
        self.RoutingService = RoutingService(self)
        self.connect(self.RoutingService, self.RoutingService.signalUpdateIpList, self.updateIpList)
        self.RoutingService.start()

#
# Main
#
if __name__ == "__main__":

    # Init app
    app = QtGui.QApplication(sys.argv)

    # Init main window
    MainWindow = GroundSystem()

    # Show and put window on front
    MainWindow.show()
    MainWindow.raise_()

    # Start the Routing Service
    MainWindow.initRoutingService()

    # Execute the app
    sys.exit(app.exec_())

