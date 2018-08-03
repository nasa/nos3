# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'EventMessageDialog.ui'
#
# Created: Thu Mar 26 11:45:17 2015
#      by: PyQt4 UI code generator 4.11.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    def _fromUtf8(s):
        return s

try:
    _encoding = QtGui.QApplication.UnicodeUTF8
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig, _encoding)
except AttributeError:
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig)

class Ui_EventMessageDialog(object):
    def setupUi(self, EventMessageDialog):
        EventMessageDialog.setObjectName(_fromUtf8("EventMessageDialog"))
        EventMessageDialog.resize(591, 277)
        self.verticalLayout = QtGui.QVBoxLayout(EventMessageDialog)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.horizontalLayout = QtGui.QHBoxLayout()
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.label_2 = QtGui.QLabel(EventMessageDialog)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_2.sizePolicy().hasHeightForWidth())
        self.label_2.setSizePolicy(sizePolicy)
        self.label_2.setMinimumSize(QtCore.QSize(91, 17))
        self.label_2.setMaximumSize(QtCore.QSize(91, 17))
        self.label_2.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.horizontalLayout.addWidget(self.label_2)
        self.sequenceCount = QtGui.QTextBrowser(EventMessageDialog)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.sequenceCount.sizePolicy().hasHeightForWidth())
        self.sequenceCount.setSizePolicy(sizePolicy)
        self.sequenceCount.setMinimumSize(QtCore.QSize(81, 31))
        self.sequenceCount.setMaximumSize(QtCore.QSize(81, 31))
        self.sequenceCount.setObjectName(_fromUtf8("sequenceCount"))
        self.horizontalLayout.addWidget(self.sequenceCount)
        spacerItem = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem)
        self.line = QtGui.QFrame(EventMessageDialog)
        self.line.setFrameShape(QtGui.QFrame.VLine)
        self.line.setFrameShadow(QtGui.QFrame.Sunken)
        self.line.setObjectName(_fromUtf8("line"))
        self.horizontalLayout.addWidget(self.line)
        self.label = QtGui.QLabel(EventMessageDialog)
        self.label.setObjectName(_fromUtf8("label"))
        self.horizontalLayout.addWidget(self.label)
        self.line_2 = QtGui.QFrame(EventMessageDialog)
        self.line_2.setFrameShape(QtGui.QFrame.VLine)
        self.line_2.setFrameShadow(QtGui.QFrame.Sunken)
        self.line_2.setObjectName(_fromUtf8("line_2"))
        self.horizontalLayout.addWidget(self.line_2)
        spacerItem1 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem1)
        spacerItem2 = QtGui.QSpacerItem(81, 31, QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem2)
        self.buttonBox = QtGui.QDialogButtonBox(EventMessageDialog)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.buttonBox.sizePolicy().hasHeightForWidth())
        self.buttonBox.setSizePolicy(sizePolicy)
        self.buttonBox.setMinimumSize(QtCore.QSize(91, 32))
        self.buttonBox.setMaximumSize(QtCore.QSize(91, 32))
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Close)
        self.buttonBox.setObjectName(_fromUtf8("buttonBox"))
        self.horizontalLayout.addWidget(self.buttonBox)
        self.verticalLayout.addLayout(self.horizontalLayout)
        self.eventOutput = QtGui.QTextBrowser(EventMessageDialog)
        self.eventOutput.setObjectName(_fromUtf8("eventOutput"))
        self.verticalLayout.addWidget(self.eventOutput)

        self.retranslateUi(EventMessageDialog)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("accepted()")), EventMessageDialog.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("rejected()")), EventMessageDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(EventMessageDialog)

    def retranslateUi(self, EventMessageDialog):
        EventMessageDialog.setWindowTitle(_translate("EventMessageDialog", "Event Messages", None))
        self.label_2.setText(_translate("EventMessageDialog", "Packet Count", None))
        self.label.setText(_translate("EventMessageDialog", "Events", None))


if __name__ == "__main__":
    import sys
    app = QtGui.QApplication(sys.argv)
    EventMessageDialog = QtGui.QDialog()
    ui = Ui_EventMessageDialog()
    ui.setupUi(EventMessageDialog)
    EventMessageDialog.show()
    sys.exit(app.exec_())

