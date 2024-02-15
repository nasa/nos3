from pathlib import Path
from PySide6.QtWidgets import QWidget, QApplication, QFileDialog, QTextEdit, QPushButton, QDateTimeEdit, QLabel, QCheckBox, QVBoxLayout, QSizePolicy, QDoubleSpinBox, QLayout
from PySide6.QtCore import QProcess, QDateTime
from PySide6.QtGui import QTextCharFormat

import sys, re, xmltodict, datetime, threading
import xml.etree.ElementTree as ET

from cfg_gui_ui import Ui_Form

class cfg_gui(QWidget):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        self.ui = Ui_Form()
        self.ui.setupUi(self)
        self.setFixedSize(655, 655)
        self.setWindowTitle("NOS3 Igniter - Version 0.0.1")

        self.dateTimeEdit = QDateTimeEdit()
        self.scConfigs = {}                                                     # Stores child configs {'filename' : "filetext"}
        self.prevButtonPressed = None                                           # Tracks the last button pressed, used in buttonColor()
        self.defaultStyleSheet = self.ui.pushButton_buildAll.styleSheet()       # Saves default stylesheet to return button color to normal, buttonColor()
        self.setup = 0                                                          # Allows for switchConfig() to initially be called without calling saveText()
        self.configNumTrack = 0                                                 # Tracks the index of the previous SC config when switching to another index

        # Config Tab
        self.ui.pushButton_browse.clicked.connect(self.browseConfig)
        self.ui.pushButton_save.clicked.connect(lambda: self.saveXML("save"))
        self.ui.pushButton_saveAs.clicked.connect(lambda: self.saveXML("saveAs"))
        self.ui.spinBox_configNumber.valueChanged.connect(lambda: self.switchConfig(self.ui.spinBox_configNumber.value()))
        
        # Build Tab
        self.ui.pushButton_buildAll.clicked.connect(lambda: self.build("all", self.ui.pushButton_buildAll))
        self.ui.pushButton_fswBuild.clicked.connect(lambda: self.build("fsw", self.ui.pushButton_fswBuild))
        self.ui.pushButton_gswBuild.clicked.connect(lambda: self.build("gsw", self.ui.pushButton_gswBuild))
        self.ui.pushButton_simBuild.clicked.connect(lambda: self.build("sim", self.ui.pushButton_simBuild))
        self.ui.pushButton_cleanAll.clicked.connect(lambda: self.clean("all", self.ui.pushButton_cleanAll))
        self.ui.pushButton_fswClean.clicked.connect(lambda: self.clean("fsw", self.ui.pushButton_fswClean))
        self.ui.pushButton_gswClean.clicked.connect(lambda: self.clean("gsw", self.ui.pushButton_gswClean))
        self.ui.pushButton_simClean.clicked.connect(lambda: self.clean("sim", self.ui.pushButton_simClean))

        # Launch Tab
        self.ui.pushButton_play.clicked.connect(lambda: self.startBashProcess(self.ui.textEdit_launchConsole, ["-lc", "echo '>> Starting NOS3 Time Driver'"]))
        self.ui.pushButton_stop.clicked.connect(lambda: self.gnome_terminal(self.ui.textEdit_launchConsole, "make stop"))
        self.ui.pushButton_pause.clicked.connect(lambda: self.startBashProcess(self.ui.textEdit_launchConsole, ["-lc", "echo '>> Pausing NOS3 Time Driver'"]))
        self.ui.pushButton_launch.clicked.connect(lambda: self.gnome_terminal(self.ui.textEdit_launchConsole, "make launch"))
        self.ui.comboBox_run.currentIndexChanged.connect(self.run_ForUntil)

    # Replaces the textbox on launch tab with a date/time box and vice versa
    def run_ForUntil(self):
        index = self.ui.comboBox_run.currentIndex()
        if index == 0:
            self.ui.horizontalLayout_runForUntil.itemAt(1).widget().setParent(None)
            self.ui.horizontalLayout_runForUntil.insertWidget(1, self.ui.lineEdit_secondsEntry)
            self.ui.lineEdit_secondsEntry.setPlaceholderText("")
        elif index == 1:
            self.ui.horizontalLayout_runForUntil.itemAt(1).widget().setParent(None)
            self.ui.horizontalLayout_runForUntil.insertWidget(1, self.ui.lineEdit_secondsEntry)
            self.ui.lineEdit_secondsEntry.setPlaceholderText("Seconds")
        elif index == 2:
            self.ui.horizontalLayout_runForUntil.itemAt(1).widget().setParent(None)
            self.currentTime = datetime.datetime.now()
            self.dateTimeEdit.setMinimumDateTime(QDateTime(self.currentTime.year, self.currentTime.month, self.currentTime.day, self.currentTime.hour, self.currentTime.minute, self.currentTime.second, 0, 0))
            self.ui.horizontalLayout_runForUntil.insertWidget(1, self.dateTimeEdit)
            
    # Overwrites the currently saved text for the currently selected spacecraft config when edited
    def saveText(self, layout:QLayout, config_value:int):
        child = self.scConfigs[config_value]
        text = child
        filename = text.split('\n')[0]
        childXml = text.split('\n', 2)[2]
        childXml = xmltodict.parse(childXml)

        applications = ['cf', 'ds', 'fm', 'lc', 'sc']
        components = ['adcs', 'cam', 'css', 'eps', 'fss', 'gps', 'imu', 'mag', 'radio', 'rw', 'sample', 'st', 'torquer']

        i = 0
        while layout.itemAt(i) != None:
            widget = layout.itemAt(i).widget()
            if isinstance(widget, QCheckBox):
                #print(child.text())
                text = widget.text().split(' ')[0]
                if text in applications:
                    childXml['sc-1-config']['applications'][text]['enable'] = str(widget.isChecked()).lower()
                elif text in components:
                    childXml['sc-1-config']['components'][text]['enable'] = str(widget.isChecked()).lower()
                elif text == 'gui':
                    childXml['sc-1-config'][text]['enable'] = str(widget.isChecked()).lower()
            elif isinstance(widget, QDoubleSpinBox):
                #print(child.prefix())
                prefix = widget.prefix().split(' ')[0]
                childXml['sc-1-config']['orbit'][prefix] = str(widget.value())
            i += 1
            
        combined = filename + '\n\n' + xmltodict.unparse(childXml)
        self.scConfigs[config_value] = combined
        
    # Saves the master/child XML's edited in the GUI
    def saveXML(self, saveType:str):
        # saveType = "save" (overwrite) or "saveAs" (new)

        if saveType == "saveAs":
            savePath, _ = QFileDialog.getSaveFileName(None, 'Directory', './cfg/custom')
        elif saveType == "save":
            savePath = self.config_path

        # Grab master and save to xml
        masterXml = xmltodict.parse(self.ui.textEdit_masterConfig.toPlainText())
        self.convert2xml(masterXml, savePath)

        # TODO: Now handle children, currently placeholder until checkboxes are added (update in progress)
        self.saveText(self.layout_, self.ui.spinBox_configNumber.value()-1)

        for child in self.scConfigs:
            text = str(self.scConfigs[child])
            filename = text.split('\n')[0].split(' ')[1]
            childXml = text.split('\n', 2)[2]
            childXml = xmltodict.parse(childXml)
            
            # save under same directory as masterXml using filename parsed from textEdit
            self.convert2xml(childXml, savePath.rsplit('/', 1)[0]+f'/{filename}')

    # Loads the child config into the Spacecraft Config textbox
    def switchConfig(self, value:int):
        # value : index of spacecraft config in the order listed in the master XML
        #         Note: Parameter indexing starts at 1

        # save edits made to config before viewing next one
        if self.setup == 1:
            self.saveText(self.layout_, self.configNumTrack)
            self.configNumTrack = value-1

        self.setup = 1
        self.ui.scrollArea.setWidgetResizable(True)
        self.ui.scrollAreaWidgetContents.setLayout(QVBoxLayout().layout())
        self.layout_ = self.ui.scrollAreaWidgetContents.layout()
        self.layout_.setSpacing(12)

        # remove all items from SC Config window when switching index
        while self.layout_.itemAt(0) != None:
            child = self.layout_.itemAt(0).widget().setParent(None)


        value = value-1
        if value in self.scConfigs:
            fileName = self.scConfigs[value].split('\n')[0]
            childXML = self.scConfigs[value].split('\n')[2::]
            childXML = ''.join(childXML)
            childDict = xmltodict.parse(childXML)
            
            policy = self.ui.scrollAreaWidgetContents.sizePolicy()

            for child in childDict:
                configTag = QLabel()
                configTag.setText((fileName))
                configTag.setMinimumHeight(18)
                self.layout_.addWidget(configTag)
                for child2 in childDict[child]:
                    tag = QLabel()
                    tag.setText(child2.upper()+": ")
                    format = QTextCharFormat()
                    format.setFontUnderline(True)
                    tag.setFont(format.font())
                    tag.setMinimumHeight(18)
                    self.layout_.addWidget(tag)
                    if child2 in ['applications', 'components']:
                        for child3 in childDict[child][child2]:
                            enableTag = QCheckBox()
                            enableTag.setText(child3 + " enable ")
                            enableTag.setChecked(childDict[child][child2][child3]['enable'] == 'true')
                            enableTag.setMinimumHeight(18)
                            enableTag.sizePolicy().setVerticalPolicy(QSizePolicy.Expanding)
                            self.layout_.addWidget(enableTag)
                    elif child2 == 'gui':
                        enableTag = QCheckBox()
                        enableTag.setText(child2 + " enable ")
                        enableTag.setChecked(childDict[child][child2]['enable'] == 'true')
                        self.layout_.addWidget(enableTag)
                    elif child2 == 'orbit':
                        for child3 in childDict[child][child2]:
                            orbitSpinBox = QDoubleSpinBox()
                            orbitSpinBox.setMinimum(-99.00)
                            orbitSpinBox.setMaximum(99.00)
                            orbitSpinBox.setValue(float(childDict[child][child2][child3]))
                            orbitSpinBox.setPrefix(f'{child3} = ')
                            self.layout_.addWidget(orbitSpinBox)
        else:
            tag = QLabel()
            tag.setText("*ERROR*\n\nMake sure you chose a master configuration file\n\n*ERROR*")
            self.layout_.addWidget(tag)

    # Converts a dictionary to XML file, saved under the given filename/path
    def convert2xml(self, attrDict:dict, fileName:[str, Path]):
        # ensure file is saved as xml
        if fileName[-4::] != ".xml":
            fileName += ".xml"

        # unparse dictionary to xml
        with open(fileName, "w") as f:
            xmltodict.unparse(attrDict, f, pretty=True)
            f.close()

    # Opens file selection menu and calls parseXML() on the selected file
    def browseConfig(self):
        self.config_path, _ = QFileDialog.getOpenFileName(None, 'File', './cfg', "XML Files [ *.xml ]")
        if self.config_path != "":
            self.config_name = self.config_path.split("/")[-1]
            self.ui.lineEdit_curConfig.setText(self.config_name)
            self.parseXml(self.config_path)

    # Parses Master and child XML files from the given file, updates text boxes accordingly
    def parseXml(self, config_path):
        
        # Master
        with open(config_path, 'r') as f:
            self.ui.textEdit_masterConfig.setText(f.read())
            f.close()

        i = 1
        childDict = {}
        root = ET.parse(config_path).getroot()
        for child in root:
            if child.tag == "number-spacecraft":
                 self.ui.spinBox_configNumber.setMaximum(int(child.text))
            if re.match("sc-[0-9]+-cfg", child.tag):
                 childDict[child.tag] = child.text
                 i+=1

        # Children
        config_dir = str(config_path.rsplit('/', 1)[0])
        for i, child in enumerate(childDict):
            if Path(f'{config_dir}/{childDict[child]}').is_file():
                filePath = f'{config_dir}/{childDict[child]}'
            elif Path(f'./cfg/{childDict[child]}').is_file():
                filePath = f'./cfg/{childDict[child]}'
            elif Path(f'./cfg/custom/{childDict[child]}').is_file():
                filePath = f'./cfg/custom/{childDict[child]}'
            else:
                raise FileNotFoundError(childDict[child])

            with open(filePath, 'r') as f:
                self.scConfigs[i] = f'Filename: {childDict[child]}\n\n{f.read()}'
                f.close()
                
        # Update Spacecraft Config Text
        self.switchConfig(1)

    # Starts a Bash process to execute args, redirects output to given textbox, not used for now
    def startBashProcess(self, textbox:QTextEdit, args:list):
        process = QProcess()
        process.start("bash", [item for item in args])

        process.readyReadStandardOutput.connect(lambda: textbox.append(process.readAllStandardOutput().data().decode()))
        process.readyReadStandardError.connect(lambda: textbox.append(process.readAllStandardError().data().decode()))

        process.waitForFinished()
        process.close()

    # Test for gnome-terminal instead of bash, also uses startCommand() instead of start()
    def gnome_terminal(self, textbox:QTextEdit, command:str):
        process = QProcess()
        print(command)
        process.startCommand(f'gnome-terminal -- {command}')

        process.readyReadStandardOutput.connect(lambda: textbox.append(process.readAllStandardOutput().data().decode()))
        process.readyReadStandardError.connect(lambda: textbox.append(process.readAllStandardError().data().decode()))

        process.waitForFinished(msecs=-1)
        textbox.append(f'>> {command}...')
        #process.close()

    # Placeholder clean command
    def clean(self, software:str, button:QPushButton):
        textbox = self.ui.textEdit_buildConsole
        if software == 'all':
            command = f'make clean'
        else:
            command = f'make clean-{software}'
        
        self.buttonColor(button)
        t1 = threading.Thread(target=self.thread_Bash(textbox, button, command), name='t1')
        t1.start()
        
    # Placeholder build command, assumes make prep already ran, same with clean commands
    def build(self, software:str, button:QPushButton):
        textbox = self.ui.textEdit_buildConsole
        command = f'make {software}'

        self.buttonColor(button)
        t1 = threading.Thread(target=self.thread_Bash(textbox, button, command), name='t1')
        t1.start()
    
    # Button/Bash function wrapper for threads
    def thread_Bash(self, textbox:QTextEdit, button:QPushButton, command:str):
        self.disableButtons(button)
        self.gnome_terminal(textbox, command)
        self.enableButtons(button)

    # Changes the color of the most recently pressed button to green
    def buttonColor(self, button:QPushButton):
        if self.prevButtonPressed is not None:
            self.prevButtonPressed.setStyleSheet(self.defaultStyleSheet)
        button.setStyleSheet('QPushButton {background-color: green;}')
        self.prevButtonPressed = button

    # Disable build/clean buttons while another is being ran
    def disableButtons(self, button:QPushButton):
        index = self.ui.gridLayout_buildCleanButtons.count()-1
        while index >= 0:
            widget = self.ui.gridLayout_buildCleanButtons.itemAt(index).widget()
            if widget != button:
                widget.setDisabled(1)
            index -= 1

    # Enable build/clean buttons after process is done running
    def enableButtons(self, button:QPushButton):
        index = self.ui.gridLayout_buildCleanButtons.count()-1
        while index >= 0:
            widget = self.ui.gridLayout_buildCleanButtons.itemAt(index).widget()
            if widget != button:
                widget.setEnabled(1)
            index -= 1


def main():
    app = QApplication(sys.argv)
    win = cfg_gui()
    win.show()
    sys.exit(app.exec())

main()