import sys
import csv
import getopt 
import subprocess 
import shlex
import re
import pickle

from PyQt4 import QtGui, QtNetwork
from ParameterDialog import Ui_Dialog
from HTMLParser import HTMLParser
from HTMLDocsParser import HTMLDocsParser
from struct import *

class Parameter(QtGui.QDialog):

    #
    # Initializes Parameter class
    #
    def __init__(self):
        QtGui.QDialog.__init__(self) # calls QDialog constructor
        self.parser = HTMLDocsParser() # creates instance of HTMLDocsParser class
        self.ui = Ui_Dialog() # creates instance of Ui_Dialog class, sets to var ui
        self.ui.setupUi(self) # calls setupUi method in Ui_Dialog class to setup UI
        self.setWindowTitle("Parameter Dialog") # sets title of dialog box
        self.ui.SendButton_1.clicked.connect(self.ProcessSendButton) # connects button to method

    # 
    # Button method
    #
    def ProcessSendButton(self):
        field1 = self.ui.input_1
        field2 = self.ui.input_2
        field3 = self.ui.input_3
        field4 = self.ui.input_4
        field5 = self.ui.input_5
        field6 = self.ui.input_6
        field7 = self.ui.input_7
        field8 = self.ui.input_8
        field9 = self.ui.input_9
        field10 = self.ui.input_10

        input1 = str(field1.toPlainText()).strip()
        input2 = str(field2.toPlainText()).strip()
        input3 = str(field3.toPlainText()).strip()
        input4 = str(field4.toPlainText()).strip()
        input5 = str(field5.toPlainText()).strip()
        input6 = str(field6.toPlainText()).strip()
        input7 = str(field7.toPlainText()).strip()
        input8 = str(field8.toPlainText()).strip()
        input9 = str(field9.toPlainText()).strip()
        input10 = str(field10.toPlainText()).strip()

        input_list = [input1, input2, input3, input4, input5, input6, input7, input8, input9, input10]

        i = 0
        param_list = []
        while input_list[i] != '':
            dataType = dataTypesNew[i]
            if dataType == '--string':
                param_list.append(dataType+'=\"'+stringLen[i]+':'+input_list[i]+'\"') 
            else:
                param_list.append(dataType+'='+input_list[i]) # --byte=4
            i += 1
        param_string = ' '.join(param_list)
        # print param_string
        prog = '../cmdUtil/cmdUtil'
        launch_string = prog + ' --host=' + pageAddress + ' --port=' + str(pagePort) + ' --pktid=' + str(pagePktId) + ' --endian=' + pageEndian + ' --cmdcode=' + cmdCode + ' ' + param_string.strip()
        # print launch_string
        cmd_args = shlex.split(launch_string)
        subprocess.Popen(cmd_args)
        self.ui.status_box.setText('Command sent!')


# 
# Main method
#
if __name__ == '__main__':

    #
    # Initializes variables
    #
    subsysTitle = ''
    cmdDesc = ''
    idx = 0
    pagePktId = 0
    param_file = 'struct_c_f_e___e_s___start_app_cmd__t.html'
    pageEndian = ''
    pageAddress = ''
    pagePort = 0
    cmdCode = ''
    params = ''


    #
    # Process command line arguments
    #                           
    opts, args = getopt.getopt(sys.argv[1:], "tdihppecf", ["title=", "descrip=", "idx=", "host=", "port=", "pktid=", "endian=", "cmdcode=","file="])

    for opt, arg in opts:
        if opt in ("-t", "--title"): 
            subsysTitle = arg # subsystem name, eg Telemetry
        elif opt in ("-d", "--descrip"): 
            cmdDesc = arg # command name, eg No-Op
        elif opt in ("-i", "--idx"): 
            idx = int(arg) # comand index in command definition file
        elif opt in ("-h", "--host"): 
            pageAddress = arg # send to address
        elif opt in ("-p", "--port"): 
            pagePort = arg # port number
        elif opt in ("-p", "--pktid"): 
            pagePktId = arg # packet ID
        elif opt in ("-e", "--endian"): 
            pageEndian = arg # LE or BE
        elif opt in ("-c", "--cmdcode"):
            cmdCode = arg # command code
        elif opt in ("-f", "--file"):
            param_file = arg # parameter definition file


    #
    # Initializes QT application and Parameter class
    #
    app = QtGui.QApplication(sys.argv) # creates instance of QtApplication class
    param = Parameter() # creates instance of Parameter class


    #
    # Gets parameter information from pickle files
    #
    pickle_file = 'ParameterFiles/' + re.split('\.', param_file)[0]
    with open(pickle_file,'rb') as pickle_obj:
        dataTypesOrig, paramNames, paramLen, paramDesc, dataTypesNew, stringLen = pickle.load(pickle_obj)

    #
    # Sets text in GUI
    #
    param.ui.subSystemTextBrowser.setText(subsysTitle) # subsystem name
    param.ui.commandAddressTextBrowser.setText(cmdDesc+' Command') # command name

    try:
        param.ui.paramName_1.setText(paramNames[0])
        param.ui.descrip_1.setText(paramDesc[0])
    except IndexError:
        pass

    try:
        param.ui.paramName_2.setText(paramNames[1])
        param.ui.descrip_2.setText(paramDesc[1])
    except IndexError:
        pass
    try:
        param.ui.paramName_3.setText(paramNames[2])
        param.ui.descrip_3.setText(paramDesc[2])
    except IndexError:
        pass
    try:
        param.ui.paramName_4.setText(paramNames[3])
        param.ui.descrip_4.setText(paramDesc[3])
    except IndexError:
        pass
    try:
        param.ui.paramName_5.setText(paramNames[4])
        param.ui.descrip_5.setText(paramDesc[4])
    except IndexError:
        pass
    try:
        param.ui.paramName_6.setText(paramNames[5])
        param.ui.descrip_6.setText(paramDesc[5])
    except IndexError:
        pass
    try:
        param.ui.paramName_7.setText(paramNames[6])
        param.ui.descrip_7.setText(paramDesc[6])
    except IndexError:
        pass
    try:
        param.ui.paramName_8.setText(paramNames[7])
        param.ui.descrip_8.setText(paramDesc[7])
    except IndexError:
        pass
    try:
        param.ui.paramName_9.setText(paramNames[8])
        param.ui.descrip_9.setText(paramDesc[8])
    except IndexError:
        pass
    try:
        param.ui.paramName_10.setText(paramNames[9])
        param.ui.descrip_10.setText(paramDesc[9])
    except IndexError:
        pass


    #
    # Displays the dialog
    #
    param.show()
    param.raise_()
    sys.exit(app.exec_())

