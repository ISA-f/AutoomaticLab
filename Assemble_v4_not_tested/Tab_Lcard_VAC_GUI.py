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

def str_to_channel_data(data, column_name):
    try:
        ind = dstr_to_channel[column_name]
        if ind == -1:
            return np.arange(data.shape[0])
        return data[:, ind]
    except Exception as e:
        return np.arange(data.shape[0])

class LcardVACPlot_Interface(object):
        def __init__(self, Lcard_device):
            print("Lcard VAC __init__ call")
            self.myLcard_IFFB = Lcard_IF_FullBuffers.Lcard_Interface_FullBuffers(Lcard_device, self.onDataUpdate)
            self.BufferUpdateTime = 0.1
            self.Y_x_plot = None
            self.LastData = pd.DataFrame()
            print("Lcard VAC __init__ executed")

        def onDataUpdate(self, Data):
            self.LastData = Data
            self._updatePlot()

        def _updatePlot(self):
            if self.Y_x_plot is None:
                return
            if self.LastData is None:
                return
            start, end = 0, -1
            try:
                x_label = self.PlotXAxis_ComboBox.currentText()
                y_label = self.PlotYAxis_ComboBox.currentText()
                self.Y_x_plot.setAxisLabel(x_label, y_label)
                
                start = int(self.QLineEdit_ShownData_StartIndex.text())
                end = int(self.QLineEdit_ShownData_EndIndex.text())
                x_data = str_to_channel_data(self.LastData, x_label)[start:end]
                y_data = str_to_channel_data(self.LastData, y_label)[start:end]
                self.Y_x_plot.update_plot(x_data, y_data)
            except Exception as e:
                print("LVAC._updatePlot", e)
            self.QLineEdit_ShownData_StartIndex.setText(str(start))
            self.QLineEdit_ShownData_EndIndex.setText(str(end))
            return

        def _updateIsActiveInterface(self):
            self.QpushButton_Start.setEnabled(not(self.myLcard_IFFB.getIsActiveInterface()))
            self.QpushButton_Stop.setEnabled(self.myLcard_IFFB.getIsActiveInterface())

        def pushStartButton(self):
            print("LVAC.pushStartButton call")
            self.myLcard_IFFB.startFullBuffersRead(self.BufferUpdateTime)
            self._updateIsActiveInterface()
            print("LVAC.pushStartButton executed")
            
        def pushStopButton(self):
            print("LVAC.pushStopButton call")
            self.myLcard_IFFB.finishFullBuffersRead()
            self._updateIsActiveInterface()
            print("LVAC.pushStopButton executed")

        def pushSaveButton(self):
            print("LVAC.pushSaveButton call")
            s = self.QLineEdit_Save.text()
            try:
                start = int(self.QLineEdit_ShownData_StartIndex.text())
                end = int(self.QLineEdit_ShownData_EndIndex.text())
                (self.LastData[start : end]).to_excel(s)
                #content =
                #file = open(s, "ab")
                #file.write(content)
                #file.close()
            except Exception as e:
                print("LVAC.pushSaveButton", e)
            print("LVAC.pushSaveButton executed")
            return

        def pushClearButton(self):
            self.myLcard_IFFB.clearData()
            return

        def setupUI(self):
            print("LVAC.setupUI call")
            self.centralwidget = QtWidgets.QWidget()
            self._translate = QtCore.QCoreApplication.translate
        # --- Plot ---
            self.myPlotWidget = QtWidgets.QWidget(self.centralwidget)
            self.myPlotWidget.setGeometry(QtCore.QRect(0, 0, 900, 600))
            self.Y_x_plot = Updatable_QTCanvas.PyplotWidget(parent = self.myPlotWidget)
            self.Y_x_plot.axes.set_title("Lcard")
            vbox_plot = QtWidgets.QVBoxLayout()
            vbox_plot.addWidget(self.Y_x_plot)
            self.myPlotWidget.setLayout(vbox_plot)
            self.myPlotWidget.setMinimumSize(700,700)
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
            self.QpushButton_Save.setObjectName("Save Button")
            self.QpushButton_Save.setText(self._translate("MainWindow", "Save as"))
            self.QpushButton_Save.setEnabled(True)
        # --- Save Filename Line ---
            self.QLineEdit_Save = QtWidgets.QLineEdit(parent = self.centralwidget)
            self.QLineEdit_Save.setStyleSheet("font: 75 12pt \"Tahoma\";")
            self.QLineEdit_Save.setText(("Lcard_VAC_" + str(datetime.now()) + ".xlsx").replace(":","_"))
            self.QLineEdit_Save.setObjectName("Save Line")

        # --- Clear ---
            self.QpushButton_Clear = QtWidgets.QPushButton(self.centralwidget)
            self.QpushButton_Clear.setStyleSheet("font: 75 18pt \"Tahoma\";")
            self.QpushButton_Clear.setObjectName("Clear")
            self.QpushButton_Clear.setText(self._translate("MainWindow", "Clear"))
            self.QpushButton_Clear.setEnabled(True)

        # --- Layout ---
            hbox_Start_Stop = QtWidgets.QHBoxLayout()
            hbox_Start_Stop.addWidget(self.QpushButton_Start)
            hbox_Start_Stop.addWidget(self.QpushButton_Stop)

            hbox_Save = QtWidgets.QHBoxLayout()
            hbox_Save.addWidget(self.QpushButton_Save)
            hbox_Save.addWidget(self.QLineEdit_Save)

            vbox_Controls = QtWidgets.QVBoxLayout()
            vbox_Controls.addLayout(self.QLayout_PlotComboBoxes)
            vbox_Controls.addLayout(self.QLayout_StartEndIndex)
            vbox_Controls.addLayout(hbox_Start_Stop)
            vbox_Controls.addLayout(hbox_Save)
            vbox_Controls.addWidget(self.QpushButton_Clear)

            hbox = QtWidgets.QHBoxLayout()
            hbox.addWidget(self.myPlotWidget)
            hbox.addLayout(vbox_Controls)

            self.centralwidget.setLayout(hbox)

        # --- Connections ---
            self.QpushButton_Start.clicked.connect(self.pushStartButton)
            self.QpushButton_Stop.clicked.connect(self.pushStopButton)
            self.QpushButton_Save.clicked.connect(self.pushSaveButton)
                
            self.QLineEdit_ShownData_StartIndex.editingFinished.connect(self._updatePlot)
            self.QLineEdit_ShownData_EndIndex.editingFinished.connect(self._updatePlot)
            self.PlotXAxis_ComboBox.currentTextChanged.connect(self._updatePlot)
            self.PlotYAxis_ComboBox.currentTextChanged.connect(self._updatePlot)
            print("LVAC.setupUI executed")
            return self.centralwidget



def test():
    print("Lcard VAC GUI test")
    import sys
    import Lcard_EmptyDevice
    
    Lcard_Device = Lcard_EmptyDevice.LcardE2010B_EmptyDevice("LcardE2010B.ini")
    ui = LcardVACPlot_Interface(Lcard_Device)
    
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    LVACwidget = ui.setupUI()
    MainWindow.setCentralWidget(LVACwidget)
    MainWindow.show()
    app.exec_()
    
if __name__ == "__main__":
    try:
        test()
        print(">> success")
        print()
    except Exception as e:
        print(">>", e)
        a = input()
