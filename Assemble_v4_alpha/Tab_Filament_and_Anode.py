#------------------------ general imports -----------------------------------
import time
import pandas as pd
import numpy as np
import configparser
from datetime import datetime

#------------------------ Qt and GUI imports --------------------------------
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtCore import QTimer
from Updatable_QTCanvas import PyplotWidget
import LCD_Filament, LCD_Anode
from MainWindow_CloseEvent import MainWindow_withCloseEvent

#------------------------ Korad imports ------------------------------------
from Device_Korad import Korad
import Device_Korad as DKorad
from Korad_Interface import Korad_Interface

#------------------------ Lcard imports ------------------------------------
import LcardDataInterface as LDIF
#from Lcard_IF_ContinuousRead import Lcard_Interface_ContinuousRead

#------------------------ Other imports ----------------------------------
from CommandTable import CommandTable

def columns_to_csv_string(columns):
    s = ""
    for column in columns:
        s += str(column) + ";"
    return (s + ";CommandTable\n")

def dict_parameters_to_csv(file, parameters):
    s1 = ""
    s2 = ""
    for i in parameters.keys():
        s1 += str(i) + ";"
        s2 += str(parameters[i]) + ";"
    file.write(str.encode(s1 + "\n"))
    file.write(str.encode(s2 + "\n"))
    return file
        
        

class FilamentAnodeTab(object):
        def __init__(self, lcard_device, korad_device):
                self.IsActiveMeasurements = False
                self.timer = None
                self.CommandTable = None
                self._MeasurementsFile = None
                # devices:
                self.myLcardIF = LDIF.LcardDataInterface(lcard_device)
                self.myKorad = korad_device
                # data processing:
                self.myData = pd.DataFrame(columns = {**LDIF.LCARD_NAMES._member_map_,
                                                      **DKorad.KORAD_NAMES._member_map_}.values())
                self.myData[["Ua", "Ia", "Imin", "sigmaI"]] = None
                self.myDataColumnDict = {**LDIF.LCARD_NAMES._value2member_map_,
                                         **DKorad.KORAD_NAMES._value2member_map_,
                                         "Ua" : "Ua", "Ia" : "Ia", "Imin" : "Imin", "sigmaI" : "sigmaI"}
                # user input filenames:
                self.ControlTableConfig = "CommandTable_example.ini"
                self.LogFilename = ("Logger_FA_" + str(datetime.now()) + ".csv").replace(":","_")
                # hardcoded file for synthtetic channels:
                self.myConstants = "Constants.ini"
                config = configparser.ConfigParser()
                config.read(self.myConstants)
                self.k1 = float(config['Constants']['k1'])
                self.k2 = float(config['Constants']['k2'])
                self.c1 = float(config['Constants']['c1'])
                self.c2 = float(config['Constants']['c2'])

        def setupUi(self):
                self.centralwidget = QtWidgets.QWidget()
                # LCD Filament GUI
                self.LCD_Filament_widget = QtWidgets.QWidget(self.centralwidget)
                self.LCD_Filament_widget.setGeometry(QtCore.QRect(0, 0, 600, 600))
                self.myLCD_Filament = LCD_Filament.LCD_Filament(self.LCD_Filament_widget)
                self.myLCD_Filament.SetupUI(self.LCD_Filament_widget)
                # LCD Anode GUI
                self.LCD_Anode_widget = QtWidgets.QWidget(self.centralwidget)
                self.LCD_Anode_widget.setGeometry(QtCore.QRect(300, 0, 900, 900))
                self.myLCD_Anode = LCD_Anode.LCD_Anode(self.LCD_Anode_widget)
                self.myLCD_Anode.SetupUI(self.LCD_Anode_widget)
                # CommandTable : Start-Stop Buttons
                self.pushButton_start = QtWidgets.QPushButton(self.centralwidget)
                self.pushButton_start.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.pushButton_start.setObjectName("pushButton_start")
                self.pushButton_start.setGeometry(QtCore.QRect(400, 850, 500, 50))
                self.pushButton_stop = QtWidgets.QPushButton(self.centralwidget)
                self.pushButton_stop.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.pushButton_stop.setObjectName("pushButton_stop")
                self.pushButton_stop.setGeometry(QtCore.QRect(400, 900, 500, 50))
                self.pushButton_start.setText("Start Command Table")
                self.pushButton_stop.setText("Stop Command Table")
                # CommandTable : Filename
                self.QLabel_CommandTableFilename = QtWidgets.QLabel("Command Table Filename:", self.centralwidget)
                self.QLabel_CommandTableFilename.setGeometry(QtCore.QRect(270, 750, 500, 40))
                self.QLabel_CommandTableFilename.setStyleSheet("font: 75 15pt \"Tahoma\";")
                self.QLineEdit_CommandTableFilename = QtWidgets.QLineEdit(parent = self.centralwidget)
                self.QLineEdit_CommandTableFilename.setGeometry(QtCore.QRect(650, 750, 500, 50))
                self.QLineEdit_CommandTableFilename.setStyleSheet("font: 75 12pt \"Tahoma\";")
                self.QLineEdit_CommandTableFilename.setText("CommandTable_example.ini")
                # Log : Filename
                self.QLabel_LogFilename = QtWidgets.QLabel("Log Filename:", self.centralwidget)
                self.QLabel_LogFilename.setGeometry(QtCore.QRect(270, 800, 500, 40))
                self.QLabel_LogFilename.setStyleSheet("font: 75 15pt \"Tahoma\";")
                self.QLineEdit_LogFilename = QtWidgets.QLineEdit(parent = self.centralwidget)
                self.QLineEdit_LogFilename.setGeometry(QtCore.QRect(650, 800, 500, 50))
                self.QLineEdit_LogFilename.setStyleSheet("font: 75 12pt \"Tahoma\";")
                self.QLineEdit_LogFilename.setText(self.LogFilename)
                
                #self.QLayoutCommandTable = QtWidgets.QVBoxLayout(self.centralwidget)
                #self.QLayoutCommandTable.addWidget(self.pushButton_start)
                #self.QLayoutCommandTable.addWidget(self.pushButton_stop)
                #Plot
                self.myPlotWidget = QtWidgets.QWidget(self.centralwidget)
                self.myPlotWidget.setGeometry(QtCore.QRect(700, 0, 600, 600))
                self.Y_x_plot = PyplotWidget()
                vbox = QtWidgets.QVBoxLayout()
                vbox.addWidget(self.Y_x_plot)
                self.myPlotWidget.setLayout(vbox)
                self.Y_x_plot.setObjectName("Y(x) plot")
                #Plot ComboBoxes
                self.PlotXAxis_Label = QtWidgets.QLabel("X axis", self.centralwidget)
                self.PlotXAxis_Label.setGeometry(QtCore.QRect(720, 600, 300, 50))
                self.PlotXAxis_ComboBox = QtWidgets.QComboBox(self.centralwidget)
                self.PlotXAxis_ComboBox.setGeometry(QtCore.QRect(800, 600, 300, 40))
                self.PlotXAxis_ComboBox.addItems(self.myDataColumnDict.keys())
                self.PlotYAxis_Label = QtWidgets.QLabel("Y axis", self.centralwidget)
                self.PlotYAxis_Label.setGeometry(QtCore.QRect(720, 640, 300, 50))
                self.PlotYAxis_ComboBox = QtWidgets.QComboBox(self.centralwidget)
                self.PlotYAxis_ComboBox.setGeometry(QtCore.QRect(800, 640, 300, 40))
                self.PlotYAxis_ComboBox.addItems(self.myDataColumnDict.keys())
                #self.QLayout_PlotComboBoxes = QtWidgets.QGridLayout()
                #self.QLayout_PlotComboBoxes.addWidget(self.PlotXAxis_Label, 0,0)
                #self.QLayout_PlotComboBoxes.addWidget(self.PlotXAxis_ComboBox, 0,1)
                #self.QLayout_PlotComboBoxes.addWidget(self.PlotYAxis_Label, 1,0)
                #self.QLayout_PlotComboBoxes.addWidget(self.PlotYAxis_ComboBox, 1,1)

                """
                #Korad
                self.myKoradwidget = QtWidgets.QWidget(self.centralwidget)
                self.myKoradwidget.setObjectName("Korad widget")
                self.myKoradwidget.setGeometry(QtCore.QRect(0, 400, 300, 300))
                self.myKoradInterface = Korad_Interface(self.myKoradwidget)
                self.myKoradInterface.SetupUI()
                """
                #Layouts
                #self.QLayoutLCDs = QtWidgets.QHBoxLayout(self.centralwidget)
                #self.QLayoutLCDs.addWidget(self.LCD_Filament_widget)
                #self.QLayoutLCDs.addWidget(self.LCD_Anode_widget)
                #self.QLayout_FA = QtWidgets.QGridLayout()
                #self.QLayout_FA.addLayout(self.QLayoutLCDs, 0,0)
                #self.QLayout_FA.addLayout(self.QLayoutCommandTable, 0,1)
                #self.QLayout_FA.addWidget(self.myPlotWidget, 1,0)
                #self.QLayout_FA.addLayout(self.QLayout_PlotComboBoxes, 1,1)
                #self.centralwidget.setLayout(self.QLayout_FA)

                #Connections
                self.PlotXAxis_ComboBox.currentTextChanged.connect(self.updatePlot)
                self.PlotYAxis_ComboBox.currentTextChanged.connect(self.updatePlot)
                self.pushButton_start.clicked.connect(self.start_filament_anode)
                self.pushButton_stop.clicked.connect(self.stop_filament_anode)
                
                return self.centralwidget
        
        def onKoradSetI(self, value):
                self._MeasurementsFile.write(b"Korad.Set_unchecked_I ")
                self._MeasurementsFile.write(str.encode(value))
                self.myKorad.set_uncheckedI(value)

        def onKoradSetU(self, value):
                print(">>onKoradSetU", value)
                self.myKorad.set_uncheckedU(value)
                self._MeasurementsFile.write(str.encode((";"*len(self.myData.columns) + "Korad.Set_unchecked_U;" + value + ";\n")))


        def start_filament_anode(self, measurements_file):
                print("start_filament_anode")
                if self.IsActiveMeasurements:
                        return
                self.setIsActiveMeasurements(True)
                self.timer = QTimer()
                d = {"SET_I": self.onKoradSetI,
                     "SET_U": self.onKoradSetU}
                self.ControlTableConfig = self.QLineEdit_CommandTableFilename.text()
                self.LogFilename = self.QLineEdit_LogFilename.text()
                try:
                        self.CommandTable = CommandTable(config_file = self.ControlTableConfig,
                                                 dCommand_to_Functor = d,
                                                 onFinish = self.onTableFinish)
                        self._MeasurementsFile = open(self.LogFilename, "ab")
                        dict_parameters_to_csv(self._MeasurementsFile, self.myKorad.getParameters())
                        self._MeasurementsFile.write(b"\n")
                        dict_parameters_to_csv(self._MeasurementsFile, self.myLcardIF.myLcardDevice.getParameters())
                        self._MeasurementsFile.write(b"\n")
                        self._MeasurementsFile.write(str.encode(columns_to_csv_string(self.myData.columns)))
                except Exception as e:
                        print(e)
                        self.setIsActiveMeasurements(False)
                        return
                try:
                    self.myLcardIF.myLcardDevice.addListener()
                except Exception as e:
                    print(e)
                self.myKorad.StartExperiment()
                self.CommandTable.startTableExecution()
                self.timer.timeout.connect(self.update_filament_anode)
                self.timer.start(20)

        def update_filament_anode(self):
                print("update_filament_anode call")
                # receive data from Korad 
                korad_data = self.myKorad.TakeMeasurements()
                #print(korad_data)
                # receive data from Lcard
                self.myLcardIF.readBuffer()
                #print("update_filament_anode 2")
                LDIF.cropToRequestedBuffer(self.myLcardIF, 
                                            requested_buffer_size = 8000)
                #print("update_filament_anode 3")
                LDIF.calculateAverage(self.myLcardIF)
                #print("update_filament_anode 4", self.myLcardIF.data)
                lcard_data = self.myLcardIF.data
                # data processing
                myDataPiece = (pd.concat([korad_data, lcard_data])).to_frame().T
                # data processing : update synth channel
                # Тут формулки, их желательно проверить на корректность
                myDataPiece[["Ua", "Ia", "Imin", "sigmaI"]] = None
                if not(lcard_data[LDIF.LCARD_NAMES.CH0MEAN] is None):
                        myDataPiece["Ua"] = self.k1*lcard_data[LDIF.LCARD_NAMES.CH0MEAN]                     # Ua = k1 <ch1>     
                if not(None in [lcard_data[LDIF.LCARD_NAMES.CH0MEAN], lcard_data[LDIF.LCARD_NAMES.CH1MEAN]]):   # Ia = c1 <ch1> - c2 <ch2>
                        myDataPiece["Ia"] = self.c1*lcard_data[LDIF.LCARD_NAMES.CH0MEAN] - self.c2*lcard_data[LDIF.LCARD_NAMES.CH1MEAN]
                if not(None in [lcard_data[LDIF.LCARD_NAMES.CH0MIN], lcard_data[LDIF.LCARD_NAMES.CH1MAX]]):     # Imin = c1 ch1_min - c2 ch2_max
                        myDataPiece["Imin"] = self.c1*lcard_data[LDIF.LCARD_NAMES.CH0MIN] - self.c2*lcard_data[LDIF.LCARD_NAMES.CH1MAX]
                if not(None in [lcard_data[LDIF.LCARD_NAMES.CH0STD], lcard_data[LDIF.LCARD_NAMES.CH1STD]]):     # sigma = c1 sigma_1 - c2 sigma_2
                        myDataPiece["sigmaI"] = np.sqrt((self.c1*lcard_data[LDIF.LCARD_NAMES.CH0STD])**2 + (self.c2*lcard_data[LDIF.LCARD_NAMES.CH1STD])**2)
                self.myData = pd.concat([self.myData, myDataPiece])
                # data processing : save to file
                if not(self._MeasurementsFile.closed):
                        np.savetxt(self._MeasurementsFile, myDataPiece, fmt = '%s', delimiter = ";")
                # update GUIs : Plot and LCDs
                self.updatePlot()
                self.myLCD_Filament.Update_U_I(korad_data[DKorad.KORAD_NAMES.VOLTAGE],
                                                korad_data[DKorad.KORAD_NAMES.CURRENT])
                self.myLCD_Anode.Display(myDataPiece["Ua"],
                                            myDataPiece["Ia"],
                                            myDataPiece["Imin"],
                                            myDataPiece["sigmaI"])
                print("update_filament_anode executed")

        def updatePlot(self):
                amount = 200
                x_label = self.myDataColumnDict[self.PlotXAxis_ComboBox.currentText()]
                y_label = self.myDataColumnDict[self.PlotYAxis_ComboBox.currentText()]
                Y_x = self.myData[[x_label, y_label]].dropna()
                self.Y_x_plot.update_plot(Y_x[x_label][max(0, Y_x.shape[0] - amount):Y_x.shape[0]],
                                          Y_x[y_label][max(0, Y_x.shape[0] - amount):Y_x.shape[0]])
                self.Y_x_plot.setAxisLabel(self.PlotXAxis_ComboBox.currentText(),
                                           self.PlotYAxis_ComboBox.currentText())
                

        def onCloseEvent(self):
                print("Disconnecting from all devices")
                try:
                        self.setIsActiveMeasurements(False)
                        if self.CommandTable:
                                self.CommandTable.interruptTableExecution()
                        if self.timer:
                                self.timer.stop()
                        if self.myKorad:
                                self.myKorad.DisconnectFromPhysicalDevice()
                        if self.myLcardIF and self.myLcardIF.myLcardDevice:
                                self.myLcardIF.myLcardDevice.disconnectFromPhysicalDevice()
                except Exception as e:
                        print(e)

        def onTableFinish(self):
                print("FA : CommandTable Finish")
                self.stop_filament_anode()

        def stop_filament_anode(self):
            try:
                    self.setIsActiveMeasurements(False)
                    if self.timer:
                            self.timer.stop()
                    if self.CommandTable:
                            self.CommandTable.interruptTableExecution()
                    if self._MeasurementsFile and not(self._MeasurementsFile.closed):
                            self._MeasurementsFile.close()
                    if self.myKorad:
                            self.myKorad.FinishExperiment()
                    if self.myLcardIF and self.myLcardIF.myLcardDevice:
                            self.myLcardIF.myLcardDevice.removeListener()
            except Exception as e:
                    print(e)

        def setIsActiveMeasurements(self, IsActiveMeasurements: bool):
                self.IsActiveMeasurements = IsActiveMeasurements
                self.pushButton_start.setEnabled(not(IsActiveMeasurements))
                self.pushButton_stop.setEnabled(IsActiveMeasurements)
                self.QLineEdit_CommandTableFilename.setEnabled(not(IsActiveMeasurements))
                self.QLineEdit_LogFilename.setEnabled(not(IsActiveMeasurements))
                return




def test():
    print("filament_and_anode test")
    import sys 
    import Lcard_EmptyDevice
    
    myLcard = Lcard_EmptyDevice.LcardE2010B_EmptyDevice("LcardE2010B.ini")
    myKorad = Korad('Korad.ini')
    myKorad.ConnectToPhysicalDevice()
    myKorad.StartExperiment()

    app = QtWidgets.QApplication(sys.argv)
    MainWindow = MainWindow_withCloseEvent()
    ui = FilamentAnodeTab(lcard_device = myLcard, 
                          korad_device = myKorad)
    centralwidget = ui.setupUi()
    MainWindow.setCentralWidget(centralwidget)
    MainWindow.CloseEventListeners.append(ui.onCloseEvent)
    MainWindow.resize(1300,1000)
    MainWindow.show()
    app.exec_()

if __name__ == "__main__":
    try:
        test()
        print(">> success")
    except Exception as e:
        print(e)
        a = input()
    
