#------------------------ general imports -----------------------------------
import time
import pandas as pd
import numpy as np
import configparser

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



class FilamentAnodeTab(object):
        
        def __init__(self, log_file, lcard_device, korad_device, ControlTableConfig = "CommandTable_example.ini"):
                self.ControlTableConfig = ControlTableConfig
                self.myLcardIF = LDIF.LcardDataInterface(lcard_device)
                self.myKorad = korad_device
                self.myConstants = "Constants.ini"
                self.LogFile = log_file
                self.myData = pd.DataFrame(columns = {**LDIF.LCARD_NAMES._member_map_,
                                                      **DKorad.KORAD_NAMES._member_map_}.values())
                self.myData[["Ua", "Ia", "Imin", "sigmaI"]] = None
                self.myDataColumnDict = {**LDIF.LCARD_NAMES._value2member_map_,
                                         **DKorad.KORAD_NAMES._value2member_map_,
                                         "Ua" : "Ua", "Ia" : "Ia", "Imin" : "Imin", "sigmaI" : "sigmaI"}
                config = configparser.ConfigParser()
                config.read(self.myConstants)
                self.k1 = float(config['Constants']['k1'])
                self.k2 = float(config['Constants']['k2'])
                self.c1 = float(config['Constants']['c1'])
                self.c2 = float(config['Constants']['c2'])

                self.timer = None
                self.CommandTable = None
                self._MeasurementsFile = None

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
                # Start-Stop Buttons
                self.pushButton_start = QtWidgets.QPushButton(self.centralwidget)
                self.pushButton_start.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.pushButton_start.setObjectName("pushButton_start")
                self.pushButton_start.setGeometry(QtCore.QRect(0, 350, 500, 50))
                self.pushButton_stop = QtWidgets.QPushButton(self.centralwidget)
                self.pushButton_stop.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.pushButton_stop.setObjectName("pushButton_stop")
                self.pushButton_stop.setGeometry(QtCore.QRect(0, 400, 500, 50))
                self.pushButton_start.setText("Start Command Table")
                self.pushButton_stop.setText("Stop Command Table")
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
                #Korad
                self.myKoradwidget = QtWidgets.QWidget(self.centralwidget)
                self.myKoradwidget.setObjectName("Korad widget")
                self.myKoradwidget.setGeometry(QtCore.QRect(0, 500, 300, 300))
                self.myKoradInterface = Korad_Interface(self.myKoradwidget)
                self.myKoradInterface.SetupUI()
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

        def start_filament_anode(self, measurements_file):
                self.timer = QTimer()
                d = {"SET_I": self.myKorad.set_uncheckedI,
                     "SET_U": self.myKorad.set_uncheckedU}
                try:
                        self.CommandTable = CommandTable(config_file = self.ControlTableConfig,
                                                 dCommand_to_Functor = d,
                                                 onFinish = self.onTableFinish)
                except Exception as e:
                        print(e)
                        a = input()
                self._MeasurementsFile = open(self.LogFile, "ab")
                self.myLcardIF.myLcardDevice.addListener()
                self.myKorad.StartExperiment()
                self.CommandTable.startTableExecution()
                self.timer.timeout.connect(self.update_filament_anode)
                self.timer.start(20)

        def update_filament_anode(self):
                print("update filament anode")
                # receive data from Korad 
                korad_data = self.myKorad.TakeMeasurements()
                # receive data from Lcard
                self.myLcardIF.readBuffer()
                LDIF.cropToRequestedBuffer(self.myLcardIF, 
                                            requested_buffer_size = 8000)
                LDIF.calculateAverage(self.myLcardIF)
                lcard_data = self.myLcardIF.data
                # data processing
                myDataPiece = (pd.concat([korad_data, lcard_data])).to_frame().T
                # data processing : update synth channel
                # Тут формулки, их желательно проверить на корректность
                myDataPiece[["Ua", "Ia", "Imin", "sigmaI"]] = None
                if lcard_data[LDIF.LCARD_NAMES.CH0MEAN]:
                        myDataPiece["Ua"] = self.k1*lcard_data[LDIF.LCARD_NAMES.CH0MEAN]       # Ua = k1 <ch1>
                if lcard_data[LDIF.LCARD_NAMES.CH0MEAN] and lcard_data[LDIF.LCARD_NAMES.CH1MEAN]:   # Ia = c1 <ch1> - c2 <ch2>
                        myDataPiece["Ia"] = self.c1*lcard_data[LDIF.LCARD_NAMES.CH0MEAN] - self.c2*lcard_data[LDIF.LCARD_NAMES.CH1MEAN]
                if lcard_data[LDIF.LCARD_NAMES.CH0MIN] and lcard_data[LDIF.LCARD_NAMES.CH1MAX]:     # Imin = c1 ch1_min - c2 ch2_max
                        myDataPiece["Imin"] = self.c1*lcard_data[LDIF.LCARD_NAMES.CH0MIN] - self.c2*lcard_data[LDIF.LCARD_NAMES.CH1MAX]
                if lcard_data[LDIF.LCARD_NAMES.CH0STD] and lcard_data[LDIF.LCARD_NAMES.CH1STD]:     # sigma = c1 sigma_1 - c2 sigma_2
                        myDataPiece["sigmaI"] = self.c1*lcard_data[LDIF.LCARD_NAMES.CH0STD] - self.c2*lcard_data[LDIF.LCARD_NAMES.CH1STD]
                self.myData = pd.concat([self.myData, myDataPiece])
                # data processing : save to file
                if not(self._MeasurementsFile.closed):
                        self._MeasurementsFile.write(b"\n")
                        np.savetxt(self._MeasurementsFile, myDataPiece, fmt = '%s')

                # update GUIs : Plot and LCDs
                self.updatePlot()
                self.myLCD_Filament.Update_U_I(korad_data[DKorad.KORAD_NAMES.VOLTAGE],
                                                korad_data[DKorad.KORAD_NAMES.CURRENT])
                self.myLCD_Anode.Display(myDataPiece["Ua"],
                                            myDataPiece["Ia"],
                                            myDataPiece["Imin"],
                                            myDataPiece["sigmaI"])


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



if __name__ == "__main__":
    import sys 
    import Lcard_EmptyDevice
    
    myLcard = Lcard_EmptyDevice.LcardE2010B_EmptyDevice("LcardE2010B.ini")
    myKorad = Korad('Korad.ini')

    app = QtWidgets.QApplication(sys.argv)
    MainWindow = MainWindow_withCloseEvent()
    ui = FilamentAnodeTab(log_file = "ui_fa_test3.log", 
                          lcard_device = myLcard, 
                          korad_device = myKorad)
    centralwidget = ui.setupUi()
    MainWindow.setCentralWidget(centralwidget)
    MainWindow.CloseEventListeners.append(ui.onCloseEvent)
    MainWindow.resize(1300,1000)
    MainWindow.show()
    sys.exit(app.exec_())
