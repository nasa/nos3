#!/usr/bin/env python
# 
import sys
import csv
import subprocess 
import shlex
import pickle

from PyQt4 import QtGui
from CommandSystemDialog import Ui_CommandSystemDialog
from struct import *


class CommandSystem(QtGui.QDialog):

    #
    # Init the class
    #
    def __init__(self):
        QtGui.QDialog.__init__(self)
        self.ui = Ui_CommandSystemDialog()
        self.ui.setupUi(self)
        self.setWindowTitle('Command System Main Page')
        self.move(800,100)

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

        self.ui.quickButton_1.clicked.connect(self.QuickButton_1)
        self.ui.quickButton_2.clicked.connect(self.QuickButton_2)
        self.ui.quickButton_3.clicked.connect(self.QuickButton_3)
        self.ui.quickButton_4.clicked.connect(self.QuickButton_4)
        self.ui.quickButton_5.clicked.connect(self.QuickButton_5)
        self.ui.quickButton_6.clicked.connect(self.QuickButton_6)
        self.ui.quickButton_7.clicked.connect(self.QuickButton_7)
        self.ui.quickButton_8.clicked.connect(self.QuickButton_8)
        self.ui.quickButton_9.clicked.connect(self.QuickButton_9)
        self.ui.quickButton_10.clicked.connect(self.QuickButton_10)
        self.ui.quickButton_11.clicked.connect(self.QuickButton_11)
        self.ui.quickButton_12.clicked.connect(self.QuickButton_12)
        self.ui.quickButton_13.clicked.connect(self.QuickButton_13)
        self.ui.quickButton_14.clicked.connect(self.QuickButton_14)
        self.ui.quickButton_15.clicked.connect(self.QuickButton_15)
        self.ui.quickButton_16.clicked.connect(self.QuickButton_16)
        self.ui.quickButton_17.clicked.connect(self.QuickButton_17)
        self.ui.quickButton_18.clicked.connect(self.QuickButton_18)
        self.ui.quickButton_19.clicked.connect(self.QuickButton_19)
        self.ui.quickButton_20.clicked.connect(self.QuickButton_20)
        self.ui.quickButton_21.clicked.connect(self.QuickButton_21)
        self.ui.quickButton_22.clicked.connect(self.QuickButton_22)



    #
    # Display button wrappers
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

    #
    # Processes 'Display Page' button
    #
    def ProcessButtonGeneric(self, idx):
        if cmdPageIsValid[idx] == True:
           lineEditPktId = getattr(Command.ui, 'lineEditPktId_'+str(idx))
           lineEditAddress = getattr(Command.ui, 'lineEdit_'+str(idx))
           pktId = str(lineEditPktId.text())
           address = str(lineEditAddress.text())
           launch_string = 'python ' + cmdClass[0] + ' --title=\"' + cmdPageDesc[idx] + '\" --pktid=' + pktId + ' --file=' + cmdPageDefFile[idx] + ' --address=\"' + address + '\"' + ' --port=' + str(cmdPagePort[idx]) + ' --endian=' + cmdPageEndian[idx]
           cmd_args = shlex.split(launch_string)
           print launch_string
           subprocess.Popen(cmd_args)

    #
    # Quick button wrappers
    #
    def QuickButton_1(self):
        self.ProcessQuickButton(0)
    def QuickButton_2(self):
        self.ProcessQuickButton(1)
    def QuickButton_3(self):
        self.ProcessQuickButton(2)
    def QuickButton_4(self):
        self.ProcessQuickButton(3)
    def QuickButton_5(self):
        self.ProcessQuickButton(4)
    def QuickButton_6(self):
        self.ProcessQuickButton(5)
    def QuickButton_7(self):
        self.ProcessQuickButton(6)
    def QuickButton_8(self):
        self.ProcessQuickButton(7)
    def QuickButton_9(self):
        self.ProcessQuickButton(8)
    def QuickButton_10(self):
        self.ProcessQuickButton(9)
    def QuickButton_11(self):
        self.ProcessQuickButton(10)
    def QuickButton_12(self):
        self.ProcessQuickButton(11)
    def QuickButton_13(self):
        self.ProcessQuickButton(12)
    def QuickButton_14(self):
        self.ProcessQuickButton(13)
    def QuickButton_15(self):
        self.ProcessQuickButton(14)
    def QuickButton_16(self):
        self.ProcessQuickButton(15)
    def QuickButton_17(self):
        self.ProcessQuickButton(16)
    def QuickButton_18(self):
        self.ProcessQuickButton(17)
    def QuickButton_19(self):
        self.ProcessQuickButton(18)
    def QuickButton_20(self):
        self.ProcessQuickButton(19)
    def QuickButton_21(self):
        self.ProcessQuickButton(20)
    def QuickButton_22(self):
        self.ProcessQuickButton(21)


    #
    # Determines if command requires parameters
    #
    def checkParams(self, idx):
        pickle_file = 'ParameterFiles/' + quickParam[idx]
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
    # Processes quick button
    #
    def ProcessQuickButton(self, idx):
        if cmdPageIsValid[idx] == True and quickIndices[idx] >= 0:
           quickIdx = quickIndices[idx]
           lineEditPktId = getattr(Command.ui, 'lineEditPktId_'+str(idx))
           lineEditAddress = getattr(Command.ui, 'lineEdit_'+str(idx))
           pktId = str(lineEditPktId.text())
           address = str(lineEditAddress.text())

           # if requires parameters
           if self.checkParams(quickIdx) == True:
              prog = 'python Parameter.py'
              launch_string = prog+' --title=\"'+subsys[quickIdx]+'\" --descrip=\"'+quickCmd[quickIdx]+'\" --idx='+str(idx)+' --host=\"'+address+'\" --port='+str(quickPort[quickIdx])+' --pktid='+pktId+' --endian='+quickEndian[quickIdx]+' --cmdcode='+quickCode[quickIdx]+' --file='+quickParam[quickIdx]

           # if doesn't require parameters
           else:
               launch_string = '../cmdUtil/cmdUtil' + ' --host=\"' + address + '\" --port=' + str(quickPort[quickIdx]) + ' --pktid=' + pktId + ' --endian=' + quickEndian[quickIdx] + ' --cmdcode=' + quickCode[quickIdx]

           # print launch_string
           cmd_args = shlex.split(launch_string)
           subprocess.Popen(cmd_args)


#
# Main 
#
if __name__ == '__main__':

    #
    # Set defaults for the arguments
    #
    cmdDefFile = "command-pages.txt"

    #
    # Init the QT application and the telemetry dialog class
    #
    app = QtGui.QApplication(sys.argv)
    Command = CommandSystem()

    #
    # Read in the contents of the telemetry packet defintion
    #
    cmdPageIsValid = []
    cmdPageDesc = []
    cmdPageDefFile = []
    cmdPageAppid = []
    cmdPageEndian = []
    cmdClass = []
    cmdPageAddress = []
    cmdPagePort = []

    i = 0

    with open(cmdDefFile, 'rb') as cmdfile:
        reader = csv.reader(cmdfile, skipinitialspace = True)
        for cmdRow in reader:
            try:
                if cmdRow[0][0] != '#':
                    cmdPageIsValid.append(True)
                    cmdPageDesc.append(cmdRow[0])
                    cmdPageDefFile.append(cmdRow[1])
                    cmdPageAppid.append(int(cmdRow[2],16))
                    cmdPageEndian.append(cmdRow[3])
                    cmdClass.append(cmdRow[4])
                    cmdPageAddress.append(cmdRow[5])
                    cmdPagePort.append(int(cmdRow[6]))
                    i += 1
            except IndexError:
                print "IndexError: list index out of range"
                print "This could be due to improper formatting in command-pages.txt."
                print "This is a common error caused by blank lines in command-pages.txt"

    # 
    # Mark the remaining values as invalid
    #
    for j in range (i, 22):
       cmdPageAppid.append(0)
       cmdPageIsValid.append(False)


    # 
    # Read in contents of quick button definition file
    # 
    quickDefFile = 'quick-buttons.txt'
    subsys = []
    subsysFile = []
    quickCmd = []
    quickCode = []
    quickPktId = []
    quickEndian = []
    quickAddress = []
    quickPort = []
    quickParam = []
    quickIndices = []

    with open(quickDefFile,'rb') as subFile:
       reader = csv.reader(subFile)
       i = 0
       for fileRow in reader:
          if fileRow[0][0] != '#':
             subsys.append(fileRow[0]) 
             subsysFile.append(fileRow[1]) 
             quickCmd.append(fileRow[2].strip()) 
             quickCode.append(fileRow[3].strip()) 
             quickPktId.append(fileRow[4].strip()) 
             quickEndian.append(fileRow[5].strip()) 
             quickAddress.append(fileRow[6].strip()) 
             quickPort.append(fileRow[7].strip())
             quickParam.append(fileRow[8].strip())
             i+=1

    #
    # fill the data fields on the page
    #
    if cmdPageIsValid[0] == True:
       Command.ui.SubsysBrowser_0.setText(cmdPageDesc[0])
       Command.ui.lineEditPktId_0.setText(hex(cmdPageAppid[0]))
       Command.ui.lineEdit_0.setText(cmdPageAddress[0])
       quickIdx = -1
       try:
          quickIdx = subsys.index(cmdPageDesc[0])
          Command.ui.quickButton_1.setText(quickCmd[quickIdx])
       except ValueError:
          pass
       quickIndices.append(quickIdx)
    else:
       Command.ui.SubsysBrowser_0.setText("(unused)")

    if cmdPageIsValid[1] == True:
       Command.ui.SubsysBrowser_1.setText(cmdPageDesc[1])
       Command.ui.lineEditPktId_1.setText(hex(cmdPageAppid[1]))
       Command.ui.lineEdit_1.setText(cmdPageAddress[1])
       quickIdx = -1
       try:
          quickIdx = subsys.index(cmdPageDesc[1])
          Command.ui.quickButton_2.setText(quickCmd[quickIdx])
       except ValueError:
          pass
       quickIndices.append(quickIdx)
    else:
       Command.ui.SubsysBrowser_1.setText("(unused)")

    if cmdPageIsValid[2] == True:
       Command.ui.SubsysBrowser_2.setText(cmdPageDesc[2])
       Command.ui.lineEditPktId_2.setText(hex(cmdPageAppid[2]))
       Command.ui.lineEdit_2.setText(cmdPageAddress[2])
       quickIdx = -1
       try:
          quickIdx = subsys.index(cmdPageDesc[2])
          Command.ui.quickButton_3.setText(quickCmd[quickIdx])
       except ValueError:
          pass
       quickIndices.append(quickIdx)
    else:
       Command.ui.SubsysBrowser_2.setText("(unused)")

    if cmdPageIsValid[3] == True:
       Command.ui.SubsysBrowser_3.setText(cmdPageDesc[3])
       Command.ui.lineEditPktId_3.setText(hex(cmdPageAppid[3]))
       Command.ui.lineEdit_3.setText(cmdPageAddress[3])
       quickIdx = -1
       try:
          quickIdx = subsys.index(cmdPageDesc[3])
          Command.ui.quickButton_4.setText(quickCmd[quickIdx])
       except ValueError:
          pass
       quickIndices.append(quickIdx)
    else:
       Command.ui.SubsysBrowser_3.setText("(unused)")

    if cmdPageIsValid[4] == True:
       Command.ui.SubsysBrowser_4.setText(cmdPageDesc[4])
       Command.ui.lineEditPktId_4.setText(hex(cmdPageAppid[4]))
       Command.ui.lineEdit_4.setText(cmdPageAddress[4])
       quickIdx = -1
       try:
          quickIdx = subsys.index(cmdPageDesc[4])
          Command.ui.quickButton_5.setText(quickCmd[quickIdx])
       except ValueError:
          pass
       quickIndices.append(quickIdx)
    else:
       Command.ui.SubsysBrowser_4.setText("(unused)")

    if cmdPageIsValid[5] == True:
       Command.ui.SubsysBrowser_5.setText(cmdPageDesc[5])
       Command.ui.lineEditPktId_5.setText(hex(cmdPageAppid[5]))
       Command.ui.lineEdit_5.setText(cmdPageAddress[5])
       quickIdx = -1
       try:
          quickIdx = subsys.index(cmdPageDesc[5])
          Command.ui.quickButton_6.setText(quickCmd[quickIdx])
       except ValueError:
          pass
       quickIndices.append(quickIdx)
    else:
       Command.ui.SubsysBrowser_5.setText("(unused)")

    if cmdPageIsValid[6] == True:
       Command.ui.SubsysBrowser_6.setText(cmdPageDesc[6])
       Command.ui.lineEditPktId_6.setText(hex(cmdPageAppid[6]))
       Command.ui.lineEdit_6.setText(cmdPageAddress[6])
       quickIdx = -1
       try:
          quickIdx = subsys.index(cmdPageDesc[6])
          Command.ui.quickButton_7.setText(quickCmd[quickIdx])
       except ValueError:
          pass
       quickIndices.append(quickIdx)
    else:
       Command.ui.SubsysBrowser_6.setText("(unused)")

    if cmdPageIsValid[7] == True:
       Command.ui.SubsysBrowser_7.setText(cmdPageDesc[7])
       Command.ui.lineEditPktId_7.setText(hex(cmdPageAppid[7]))
       Command.ui.lineEdit_7.setText(cmdPageAddress[7])
       quickIdx = -1
       try:
          quickIdx = subsys.index(cmdPageDesc[7])
          Command.ui.quickButton_8.setText(quickCmd[quickIdx])
       except ValueError:
          pass
       quickIndices.append(quickIdx)
    else:
       Command.ui.SubsysBrowser_7.setText("(unused)")

    if cmdPageIsValid[8] == True:
       Command.ui.SubsysBrowser_8.setText(cmdPageDesc[8])
       Command.ui.lineEditPktId_8.setText(hex(cmdPageAppid[8]))
       Command.ui.lineEdit_8.setText(cmdPageAddress[8])
       quickIdx = -1
       try:
          quickIdx = subsys.index(cmdPageDesc[8])
          Command.ui.quickButton_9.setText(quickCmd[quickIdx])
       except ValueError:
          pass
       quickIndices.append(quickIdx)
    else:
       Command.ui.SubsysBrowser_8.setText("(unused)")

    if cmdPageIsValid[9] == True:
       Command.ui.SubsysBrowser_9.setText(cmdPageDesc[9])
       Command.ui.lineEditPktId_9.setText(hex(cmdPageAppid[9]))
       Command.ui.lineEdit_9.setText(cmdPageAddress[9])
       quickIdx = -1
       try:
          quickIdx = subsys.index(cmdPageDesc[9])
          Command.ui.quickButton_10.setText(quickCmd[quickIdx])
       except ValueError:
          pass
       quickIndices.append(quickIdx)
    else:
       Command.ui.SubsysBrowser_9.setText("(unused)")

    if cmdPageIsValid[10] == True:
       Command.ui.SubsysBrowser_10.setText(cmdPageDesc[10])
       Command.ui.lineEditPktId_10.setText(hex(cmdPageAppid[10]))
       Command.ui.lineEdit_10.setText(cmdPageAddress[10])
       quickIdx = -1
       try:
          quickIdx = subsys.index(cmdPageDesc[10])
          Command.ui.quickButton_11.setText(quickCmd[quickIdx])
       except ValueError:
          pass
       quickIndices.append(quickIdx)
    else:
       Command.ui.SubsysBrowser_10.setText("(unused)")

    if cmdPageIsValid[11] == True:
       Command.ui.SubsysBrowser_11.setText(cmdPageDesc[11])
       Command.ui.lineEditPktId_11.setText(hex(cmdPageAppid[11]))
       Command.ui.lineEdit_11.setText(cmdPageAddress[11])
       quickIdx = -1
       try:
          quickIdx = subsys.index(cmdPageDesc[11])
          Command.ui.quickButton_12.setText(quickCmd[quickIdx])
       except ValueError:
          pass
       quickIndices.append(quickIdx)
    else:
       Command.ui.SubsysBrowser_11.setText("(unused)")

    if cmdPageIsValid[12] == True:
       Command.ui.SubsysBrowser_12.setText(cmdPageDesc[12])
       Command.ui.lineEditPktId_12.setText(hex(cmdPageAppid[12]))
       Command.ui.lineEdit_12.setText(cmdPageAddress[12])
       quickIdx = -1
       try:
          quickIdx = subsys.index(cmdPageDesc[12])
          Command.ui.quickButton_13.setText(quickCmd[quickIdx])
       except ValueError:
          pass
       quickIndices.append(quickIdx)
    else:
       Command.ui.SubsysBrowser_12.setText("(unused)")

    if cmdPageIsValid[13] == True:
       Command.ui.SubsysBrowser_13.setText(cmdPageDesc[13])
       Command.ui.lineEditPktId_13.setText(hex(cmdPageAppid[13]))
       Command.ui.lineEdit_13.setText(cmdPageAddress[13])
       quickIdx = -1
       try:
          quickIdx = subsys.index(cmdPageDesc[13])
          Command.ui.quickButton_14.setText(quickCmd[quickIdx])
       except ValueError:
          pass
       quickIndices.append(quickIdx)
    else:
       Command.ui.SubsysBrowser_13.setText("(unused)")

    if cmdPageIsValid[14] == True:
       Command.ui.SubsysBrowser_14.setText(cmdPageDesc[14])
       Command.ui.lineEditPktId_14.setText(hex(cmdPageAppid[14]))
       Command.ui.lineEdit_14.setText(cmdPageAddress[14])
       quickIdx = -1
       try:
          quickIdx = subsys.index(cmdPageDesc[14])
          Command.ui.quickButton_15.setText(quickCmd[quickIdx])
       except ValueError:
          pass
       quickIndices.append(quickIdx)
    else:
       Command.ui.SubsysBrowser_14.setText("(unused)")

    if cmdPageIsValid[15] == True:
       Command.ui.SubsysBrowser_15.setText(cmdPageDesc[15])
       Command.ui.lineEditPktId_15.setText(hex(cmdPageAppid[15]))
       Command.ui.lineEdit_15.setText(cmdPageAddress[15])
       quickIdx = -1
       try:
          quickIdx = subsys.index(cmdPageDesc[15])
          Command.ui.quickButton_16.setText(quickCmd[quickIdx])
       except ValueError:
          pass
       quickIndices.append(quickIdx)
    else:
       Command.ui.SubsysBrowser_15.setText("(unused)")

    if cmdPageIsValid[16] == True:
       Command.ui.SubsysBrowser_16.setText(cmdPageDesc[16])
       Command.ui.lineEditPktId_16.setText(hex(cmdPageAppid[16]))
       Command.ui.lineEdit_16.setText(cmdPageAddress[16])
       quickIdx = -1
       try:
          quickIdx = subsys.index(cmdPageDesc[16])
          Command.ui.quickButton_17.setText(quickCmd[quickIdx])
       except ValueError:
          pass
       quickIndices.append(quickIdx)
    else:
       Command.ui.SubsysBrowser_16.setText("(unused)")

    if cmdPageIsValid[17] == True:
       Command.ui.SubsysBrowser_17.setText(cmdPageDesc[17])
       Command.ui.lineEditPktId_17.setText(hex(cmdPageAppid[17]))
       Command.ui.lineEdit_17.setText(cmdPageAddress[17])
       quickIdx = -1
       try:
          quickIdx = subsys.index(cmdPageDesc[17])
          Command.ui.quickButton_18.setText(quickCmd[quickIdx])
       except ValueError:
          pass
       quickIndices.append(quickIdx)
    else:
       Command.ui.SubsysBrowser_17.setText("(unused)")

    if cmdPageIsValid[18] == True:
       Command.ui.SubsysBrowser_18.setText(cmdPageDesc[18])
       Command.ui.lineEditPktId_18.setText(hex(cmdPageAppid[18]))
       Command.ui.lineEdit_18.setText(cmdPageAddress[18])
       quickIdx = -1
       try:
          quickIdx = subsys.index(cmdPageDesc[18])
          Command.ui.quickButton_19.setText(quickCmd[quickIdx])
       except ValueError:
          pass
       quickIndices.append(quickIdx)
    else:
       Command.ui.SubsysBrowser_18.setText("(unused)")

    if cmdPageIsValid[19] == True:
       Command.ui.SubsysBrowser_19.setText(cmdPageDesc[19])
       Command.ui.lineEditPktId_19.setText(hex(cmdPageAppid[19]))
       Command.ui.lineEdit_19.setText(cmdPageAddress[19])
       quickIdx = -1
       try:
          quickIdx = subsys.index(cmdPageDesc[19])
          Command.ui.quickButton_20.setText(quickCmd[quickIdx])
       except ValueError:
          pass
       quickIndices.append(quickIdx)
    else:
       Command.ui.SubsysBrowser_19.setText("(unused)")

    if cmdPageIsValid[20] == True:
       Command.ui.SubsysBrowser_20.setText(cmdPageDesc[20])
       Command.ui.lineEditPktId_20.setText(hex(cmdPageAppid[20]))
       Command.ui.lineEdit_20.setText(cmdPageAddress[20])
       quickIdx = -1
       try:
          quickIdx = subsys.index(cmdPageDesc[20])
          Command.ui.quickButton_21.setText(quickCmd[quickIdx])
       except ValueError:
          pass
       quickIndices.append(quickIdx)
    else:
       Command.ui.SubsysBrowser_20.setText("(unused)")

    if cmdPageIsValid[21] == True:
       Command.ui.SubsysBrowser_21.setText(cmdPageDesc[21])
       Command.ui.lineEditPktId_21.setText(hex(cmdPageAppid[21]))
       Command.ui.lineEdit_21.setText(cmdPageAddress[21])
       quickIdx = -1
       try:
          quickIdx = subsys.index(cmdPageDesc[21])
          Command.ui.quickButton_22.setText(quickCmd[quickIdx])
       except ValueError:
          pass
       quickIndices.append(quickIdx)
    else:
       Command.ui.SubsysBrowser_21.setText("(unused)")


    # 
    # Display the page 
    #
    Command.show()
    Command.raise_()
    print 'Command System started.'
    sys.exit(app.exec_())

