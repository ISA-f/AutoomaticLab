from PyQt5 import QtCore, QtGui, QtWidgets
import pandas as pd
import numpy as np
from datetime import datetime

import Updatable_QTCanvas
import Lcard_IF_FullBuffers

dstr_to_channel = {"index" : -1,
                   "channel 0" : 0,
                   "channel 1" : 1,
                   "channel 2" : 2,
                   "channel 3" : 3}

class LcardVACPlot_Interface(object):
        def __init__(self, Lcard_device, centralwidget):
                self.centralwidget = self.centralwidget = QtWidgets.QWidget()
                self._translate = QtCore.QCoreApplication.translate
                self.IsActive = False
                self.myLcard_IFFB = Lcard_IF_FullBuffers.Lcard_Interface_FullBuffers(Lcard_device)
                self.BufferUpdateTime = 0.1
                self.PlotUpdateTime = 0.1
                self.QTimer_PlotUpdate = QtCore.QTimer()
                self.QTimer_PlotUpdate.timeout.connect(self._updatePlot)

        def _updatePlot(self):
            print("LVAC._updatePlot call")
            #Chosen Yaxis label
            #Chosen Xaxis label
            data = self._getShownData()
            Y_x = self.myData[[x_label, y_label]].dropna()
            self.Y_x_plot.update_plot(Y_x[x_label][max(0, Y_x.shape[0] - amount):Y_x.shape[0]],
                                                Y_x[y_label][max(0, Y_x.shape[0] - amount):Y_x.shape[0]])
            self.Y_x_plot.setAxisLabel(x_label, y_label)
            return

        def pushStartButton(self):
            self._startVAC()
            self.QpushButton_Start.setEnabled(False)
            self.QpushButton_Stop.setEnabled(True)
            
        def pushStopButton(self):
            self._stopVAC()
            self.QpushButton_Start.setEnabled(False)
            self.QpushButton_Stop.setEnabled(True)

        def pushSaveButton(self):
            s = self.QLineEdit_Save.text()
            if not(s):
                s = "Lcard_VAC_" + str(datetime.now()) + ".txt"
            self._saveVAC(s)
            return

        def pushClearButton(self):
            self.myLcard_IFFB.clearData()
            self._updatePlot()
            return

        def _saveVAC(self, filename):
            file = open(filename, "w+")
            #content = str(self.Data[self.ShownData_StartIndex:self.ShownData_EndIndex])
            content = self._getShownData()
            file.write(content)
            file.close()
            return

        def _startVAC(self):
            self.IsActive = True
            self.myLcard_IFFB.startFullBuffersRead(self.BufferUpdateTime)
            self.QTimer_PlotUpdate.start(self.PlotUpdateTime * 1000)
            return

        def _stopVAC(self):
            self.myLcard_IFFB.finishFullBuffersRead();
            self.QTimer_PlotUpdate.stop()
            self.IsActive = False
            return

        def _getQLineStartEndIndex(self):
            try:
                s_start = self.QLineEdit_ShownData_StartIndex.text()
                s_end = self.QLineEdit_ShownData_EndIndex.text()
                start = int(s_start)
                end = int(s_end)
            except Exception as e:
                return 0, -1
            return start, end

        def _getQChosenAxisData(self):
            x_label = self.PlotXAxis_ComboBox.currentText()
            y_label = self.PlotYAxis_ComboBox.currentText()
            return dstr_to_channel[x_label], dstr_to_channel[y_label]

        def _getShownData(self):
            start, end = self._getQLineStartEndIndex()
            y, x = self._getQChosenAxisData()
            return self.myLcard_IFFB.myData[[x,y]][start, end]

        def setupUI(self):
            # --- Plot ---
                self.myPlotWidget = QtWidgets.QWidget(self.centralwidget)
                self.myPlotWidget.setGeometry(QtCore.QRect(700, 0, 600, 600))
                self.Y_x_plot = Updatable_QTCanvas.PyplotWidget(parent = self.myPlotWidget)
                vbox_plot = QtWidgets.QVBoxLayout()
                vbox_plot.addWidget(self.Y_x_plot)
                self.myPlotWidget.setLayout(vbox_plot)
                self.Y_x_plot.setObjectName("Y(x) plot")

                self.PlotXAxis_Label = QtWidgets.QLabel("X axis", self.centralwidget)
                self.PlotXAxis_ComboBox = QtWidgets.QComboBox(self.centralwidget)
                self.PlotXAxis_ComboBox.addItems(dstr_to_channel.keys())

                self.PlotYAxis_Label = QtWidgets.QLabel("Y axis", self.centralwidget)
                self.PlotYAxis_ComboBox = QtWidgets.QComboBox(self.centralwidget)
                self.PlotYAxis_ComboBox.addItems(dstr_to_channel.keys())

                self.QLayout_PlotComboBoxes = QtWidgets.QGridLayout()
                self.QLayout_PlotComboBoxes.addWidget(self.PlotXAxis_Label, 0,0)
                self.QLayout_PlotComboBoxes.addWidget(self.PlotXAxis_ComboBox, 0,1)
                self.QLayout_PlotComboBoxes.addWidget(self.PlotYAxis_Label, 1,0)
                self.QLayout_PlotComboBoxes.addWidget(self.PlotYAxis_ComboBox, 1,1)

                self.QLayout_Plot = QtWidgets.QVBoxLayout()
                self.QLayout_Plot.addWidget(self.myPlotWidget)
                self.QLayout_Plot.addLayout(self.QLayout_PlotComboBoxes)

            # --- Plot Start End indexes ---
                self.QLineEdit_ShownData_StartIndex = QtWidgets.QLineEdit(parent = self.centralwidget)
                self.QLineEdit_ShownData_StartIndex.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.QLineEdit_ShownData_StartIndex.setObjectName("Start Index Line")

                self.QLineEdit_ShownData_EndIndex = QtWidgets.QLineEdit(parent = self.centralwidget)
                self.QLineEdit_ShownData_EndIndex.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.QLineEdit_ShownData_EndIndex.setObjectName("End Index Line")

                self.QLabel_StartIndex = QtWidgets.QLabel("Data start index", parent = self.centralwidget)
                self.QLabel_EndIndex = QtWidgets.QLabel("Data end index", parent = self.centralwidget)

                self.QLayout_StartEndIndex = QtWidgets.QGridLayout()
                self.QLayout_StartEndIndex.addWidget(self.QLabel_StartIndex,0,0)
                self.QLayout_StartEndIndex.addWidget(self.QLineEdit_ShownData_StartIndex,0,1)
                self.QLayout_StartEndIndex.addWidget(self.QLabel_EndIndex,1,0)
                self.QLayout_StartEndIndex.addWidget(self.QLineEdit_ShownData_EndIndex,1,1)

            # --- Start Button ---
                self.QpushButton_Start = QtWidgets.QPushButton(self.centralwidget)
                self.QpushButton_Start.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.QpushButton_Start.setText(self._translate("MainWindow", "Start"))
                self.QpushButton_Start.setEnabled(True)

            # --- Stop Button ---
                self.QpushButton_Stop = QtWidgets.QPushButton(self.centralwidget)
                self.QpushButton_Stop.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.QpushButton_Stop.setObjectName("Stop")
                self.QpushButton_Stop.setText(self._translate("MainWindow", "Stop"))
                self.QpushButton_Stop.setEnabled(False)

            # --- Save Button --- 
                self.QpushButton_Save = QtWidgets.QPushButton(self.centralwidget)
                self.QpushButton_Save.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.QpushButton_Save.setObjectName("Save")
                self.QpushButton_Save.setText(self._translate("MainWindow", "Save as"))
                self.QpushButton_Save.setEnabled(False)
            # --- Save Filename Line ---
                self.QLineEdit_Save = QtWidgets.QLineEdit(parent = self.centralwidget)
                self.QLineEdit_Save.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.QLineEdit_Save.setObjectName("Save Line")

            # --- Clear ---
                self.QpushButton_Clear = QtWidgets.QPushButton(self.centralwidget)
                self.QpushButton_Clear.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.QpushButton_Clear.setObjectName("Clear")
                self.QpushButton_Clear.setText(self._translate("MainWindow", "Clear"))
                self.QpushButton_Clear.setEnabled(False)

            # --- Layout ---
                hbox_Start_Stop = QtWidgets.QHBoxLayout()
                hbox_Start_Stop.addWidget(self.QpushButton_Start)
                hbox_Start_Stop.addWidget(self.QpushButton_Stop)

                hbox_Save = QtWidgets.QHBoxLayout()
                hbox_Save.addWidget(self.QpushButton_Save)
                hbox_Save.addWidget(self.QLineEdit_Save)

                vbox = QtWidgets.QVBoxLayout()
                vbox.addLayout(self.QLayout_Plot)
                vbox.addLayout(self.QLayout_StartEndIndex)
                vbox.addLayout(hbox_Start_Stop)
                vbox.addLayout(hbox_Save)
                vbox.addWidget(self.QpushButton_Clear)

                self.centralwidget.setLayout(vbox)

            # --- Connections ---
                self.QpushButton_Start.clicked.connect(self.pushStartButton)
                self.QpushButton_Stop.clicked.connect(self.pushStopButton)
                self.QpushButton_Save.clicked.connect(self.pushSaveButton)
                
                self.QLineEdit_ShownData_StartIndex.editingFinished.connect(self._updatePlot)
                self.QLineEdit_ShownData_EndIndex.editingFinished.connect(self._updatePlot)
                self.PlotXAxis_ComboBox.currentTextChanged.connect(self._updatePlot)
                self.PlotYAxis_ComboBox.currentTextChanged.connect(self._updatePlot)

                return self.centralwidget

if __name__ == "__main__":
    import sys 
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    ui = LcardVACPlot_Interface(None, None)
    LVACwidget = ui.setupUI()
    MainWindow.setCentralWidget(LVACwidget)
    MainWindow.show()
    sys.exit(app.exec_())
