from PyQt5 import QtCore, QtGui, QtWidgets
import pandas as pd
import numpy as np
import Updatable_QTCanvas
from datetime import datetime

class LcardVACPlot_Interface(object):
        def __init__(self, centralwidget):
                self.centralwidget = centralwidget
                self._translate = QtCore.QCoreApplication.translate
                self.myLcard = None
                self.IsActive = False
                self.Data = None
                self.ShownData_StartIndex = 0
                self.ShownData_EndIndex = -1
        
        def SetupUI(self):
            # --- Plot ---
                self.myPlotWidget = QtWidgets.QWidget(self.centralwidget)
                self.myPlotWidget.setGeometry(QtCore.QRect(700, 0, 600, 600))
                self.Y_x_plot = Updatable_QTCanvas.PyplotWidget(parent = self.myPlotWidget)
                vbox_plot = QtWidgets.QVBoxLayout()
                vbox_plot.addWidget(self.Y_x_plot)
                self.myPlotWidget.setLayout(vbox_plot)
                self.Y_x_plot.setObjectName("Y(x) plot")

                self.PlotXAxis_Label = QtWidgets.QLabel("X axis", self.centralwidget)
                self.PlotXAxis_Label.setGeometry(QtCore.QRect(720, 600, 300, 50))

                self.PlotXAxis_ComboBox = QtWidgets.QComboBox(self.centralwidget)
                self.PlotXAxis_ComboBox.setGeometry(QtCore.QRect(800, 600, 300, 40))
                self.PlotXAxis_ComboBox.addItems(self.myData.columns)

                self.PlotYAxis_Label = QtWidgets.QLabel("Y axis", self.centralwidget)
                self.PlotYAxis_Label.setGeometry(QtCore.QRect(720, 640, 300, 50))

                self.PlotYAxis_ComboBox = QtWidgets.QComboBox(self.centralwidget)
                self.PlotYAxis_ComboBox.setGeometry(QtCore.QRect(800, 640, 300, 40))
                self.PlotYAxis_ComboBox.addItems(self.myData.columns)
                self.PlotXAxis_ComboBox.currentTextChanged.connect(self.updatePlot)
                self.PlotYAxis_ComboBox.currentTextChanged.connect(self.updatePlot)

            # --- Plot Start End indexes ---
                self.QLineEdit_ShownData_StartIndex = QtWidgets.QLineEdit(parent = self.centralwidget)
                self.QLineEdit_ShownData_StartIndex.setGeometry(QtCore.QRect(900, 140, 151, 51))
                self.QLineEdit_ShownData_StartIndex.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.QLineEdit_ShownData_StartIndex.setObjectName("Start Index Line")

                self.QLineEdit_ShownData_EndIndex = QtWidgets.QLineEdit(parent = self.centralwidget)
                self.QLineEdit_ShownData_EndIndex.setGeometry(QtCore.QRect(900, 140, 151, 51))
                self.QLineEdit_ShownData_EndIndex.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.QLineEdit_ShownData_EndIndex.setObjectName("End Index Line")

                

            # --- Start Button ---
                self.QpushButton_Start = QtWidgets.QPushButton(self.centralwidget)
                self.QpushButton_Start.setGeometry(QtCore.QRect(700, 140, 151, 51))
                self.QpushButton_Start.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.QpushButton_Start.setObjectName("Start")
                self.QpushButton_Start.setText(self._translate("MainWindow", "Start"))
                self.QpushButton_Start.setEnabled(True)

            # --- Stop Button ---
                self.QpushButton_Stop = QtWidgets.QPushButton(self.centralwidget)
                self.QpushButton_Stop.setGeometry(QtCore.QRect(700, 140, 151, 51))
                self.QpushButton_Stop.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.QpushButton_Stop.setObjectName("Stop")
                self.QpushButton_Stop.setText(self._translate("MainWindow", "Stop"))
                self.QpushButton_Stop.setEnabled(False)

            # --- Save Button --- 
                self.QpushButton_Save = QtWidgets.QPushButton(self.centralwidget)
                self.QpushButton_Save.setGeometry(QtCore.QRect(700, 140, 151, 51))
                self.QpushButton_Save.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.QpushButton_Save.setObjectName("Save")
                self.QpushButton_Save.setText(self._translate("MainWindow", "Save as"))
                self.QpushButton_Save.setEnabled(False)
            # --- Save Filename Line ---
                self.QLineEdit_Save = QtWidgets.QLineEdit(parent = self.centralwidget)
                self.QLineEdit_Save.setGeometry(QtCore.QRect(900, 140, 151, 51))
                self.QLineEdit_Save.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.QLineEdit_Save.setObjectName("Save Line")

            # --- Clear ---
                self.QpushButton_Clear = QtWidgets.QPushButton(self.centralwidget)
                self.QpushButton_Clear.setGeometry(QtCore.QRect(700, 140, 151, 51))
                self.QpushButton_Clear.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.QpushButton_Clear.setObjectName("Clear")
                self.QpushButton_Clear.setText(self._translate("MainWindow", "Clear"))
                self.QpushButton_Clear.setEnabled(False)

            # --- Layout ---
                hbox_Start_Stop = QtWidgets.QHBoxLayout()
                hbox_Start_Stop.addWidget(self.QpushButton_Start)
                hbox_Start_Stop.addWidget(self.QLineEdit_Stop)

                hbox_Save = QtWidgets.QHBoxLayout()
                hbox_Save.addWidget(self.QpushButton_Save)
                hbox_Save.addWidget(self.QLineEdit_Save)

                vbox = QtWidgets.QVBoxLayout()
                vbox.addLayout(hbox_Start_Stop)
                vbox.addLayout(hbox_Save)
                vbox.addWidget(self.QpushButton_Clear)

                self.centralwidget.setLayout(vbox)

            # --- Connections ---
                self.QpushButton_Start.clicked.connect(self.pushStartButton)
                self.QpushButton_Stop.clicked.connect(self.pushStopButton)
                self.QpushButton_Save.clicked.connect(self.pushSaveButton)

        def updatePlot(self):
            #Chosen Yaxis label
            #Chosen Xaxis label
            #Chosen Start and End index
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
                s = "default_VAC_filename" + str(datetime.now()) + ".txt"
            self._saveVAC(s)
            return

        def pushClearButton(self):
            self.Data = None
            return

        def _saveVAC(self, filename):
            file = open(filename, "w+")
            content = str(self.Data[self.ShownData_StartIndex:self.ShownData_EndIndex])
            file.write(content)
            file.close()
            return

        def _startVAC(self):
            return

        def _stopVAC(self):
            return
