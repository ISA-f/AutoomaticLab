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
                self.timer = None
                self.myConstants = "Constants.ini"
                self.LogFile = log_file
                self.myData = pd.DataFrame(np.zeros((1,17)), columns = ['Korad_time', 'Korad_U', 'Korad_I',
                                                      'Lcard_comp_time', 'Lcard_buffer_size',
                                                      'Lcard_Ch0_mean','Lcard_Ch0_var', 'Lcard_Ch0_min', 'Lcard_Ch0_max',
                                                      'Lcard_Ch1_mean','Lcard_Ch1_var', 'Lcard_Ch1_min', 'Lcard_Ch1_max',
                                                      'Lcard_synth_mean','Lcard_synth_var', 'Lcard_synth_min', 'Lcard_synth_max'])

                config = configparser.ConfigParser()
                config.read(self.myConstants)
                self.k1 = float(config['Constants']['k1'])
                self.k2 = float(config['Constants']['k2'])
                self.c1 = float(config['Constants']['c1'])
                self.c2 = float(config['Constants']['c2'])

        def setupUi(self):
                self.centralwidget = QtWidgets.QWidget()

                self.LCD_Filament_widget = QtWidgets.QWidget(self.centralwidget)
                self.LCD_Filament_widget.setGeometry(QtCore.QRect(0, 0, 500, 500))
                self.myLCD_Filament = LCD_Filament.LCD_Filament(self.LCD_Filament_widget)
                self.myLCD_Filament.SetupUI(self.LCD_Filament_widget)

                self.LCD_Anode_widget = QtWidgets.QWidget(self.centralwidget)
                self.LCD_Anode_widget.setGeometry(QtCore.QRect(300, 0, 900, 900))
                self.myLCD_Anode = LCD_Anode.LCD_Anode(self.LCD_Anode_widget)
                self.myLCD_Anode.SetupUI(self.LCD_Anode_widget)
                
                self.pushButton_start = QtWidgets.QPushButton(self.centralwidget)
                self.pushButton_start.setGeometry(QtCore.QRect(150, 440, 151, 51))
                self.pushButton_start.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.pushButton_start.setObjectName("pushButton_start")
                self.pushButton_stop = QtWidgets.QPushButton(self.centralwidget)
                self.pushButton_stop.setGeometry(QtCore.QRect(320, 440, 151, 51))
                self.pushButton_stop.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.pushButton_stop.setObjectName("pushButton_stop")

                #Plot
                self.myPlotWidget = QtWidgets.QWidget(self.centralwidget)
                self.myPlotWidget.setGeometry(QtCore.QRect(700, 0, 600, 600))
                self.Y_x_plot = PyplotWidget()
                vbox = QtWidgets.QVBoxLayout()
                vbox.addWidget(self.Y_x_plot)
                self.myPlotWidget.setLayout(vbox)
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
                
                #Korad
                self.myKoradwidget = QtWidgets.QWidget(self.centralwidget)
                self.myKoradwidget.setObjectName("Korad widget")
                self.myKoradwidget.setGeometry(QtCore.QRect(0, 500, 300, 300))
                self.myKoradInterface = Korad_Interface(self.myKoradwidget)
                self.myKoradInterface.SetupUI()

                #start-stop pushButtons
                self.pushButton_start.setText("Start")
                self.pushButton_stop.setText("Stop")
                self.pushButton_start.clicked.connect(self.start_filament_anode)
                self.pushButton_stop.clicked.connect(self.FinishMeasurements)
                
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

                self.myLcardIF.myLcardDevice.addListener()
                self.myKorad.StartExperiment()
                self.CommandTable.startTableExecution()
                self.timer.timeout.connect(self.update_filament_anode)
                self.timer.start(20)

        def update_filament_anode(self):
                print("update filament anode")
                try:
                # for filament
                        korad_data = self.myKorad.TakeMeasurements()
                        time_sistem, voltage, current = korad_data[0], korad_data[1], korad_data[2]
                except Exception as e:
                        print(e)
                print("> Korad data received")

                # for anode
                try:
                        self.myLcardIF.readBuffer()
                        print("LDIF.readBuffer executed")
                        LDIF.cropToRequestedBuffer(self.myLcardIF, 
                                                   requested_buffer_size = 8000)
                        print("LDIF.crop executed")
                        LDIF.calculateAverage(self.myLcardIF)
                        print("LDIF.average executed")
                        lcard_data = self.myLcardIF.data
                except Exception as e:
                        print(e)
                        a = input()
                try:
                        print("\n lcard data\n", lcard_data)
                        print("\n korad data\n", korad_data)
                        self.myDataPiece = np.concatenate([korad_data, lcard_data], axis = 0)
                        self.myData = pd.DataFrame(np.concatenate([self.myData.to_numpy(), self.myDataPiece.reshape(-1,1).T], axis = 0),
                                           columns = self.myData.columns)
                except Exception as e:
                        print(e)
                print(self.myData.shape)
                try:
                        self._MeasurementsFile = open(self.LogFile, "ab")
                        self._MeasurementsFile.write(b"\n")
                        np.savetxt(self._MeasurementsFile, self.myDataPiece, fmt = '%s')
                        self._MeasurementsFile.close()
                except Exception as e:
                        print(e)
                        a = input()
                lcard_time = lcard_data[0]
                ch1_mean = lcard_data[2]
                ch1_var = lcard_data[3]
                ch1_min = lcard_data[4]
                ch1_max = lcard_data[5]
                ch2_mean = lcard_data[6]
                ch2_var = lcard_data[7]
                ch2_min = lcard_data[8]
                ch2_max = lcard_data[9]

                self.updatePlot()
                
                # Тут формулки, их желательно проверить на корректность
                Ua, Ia, Imin, sigmaI = None, None, None, None
                if (ch1_mean != None):
                        Ua = self.k1*ch1_mean                                        # Ua = k1 <ch1>
                if (ch1_mean != None) and (ch2_mean != None):
                        Ia = self.c1*ch1_mean - self.c2*ch2_mean                          # Ia = c1 <ch1> - c2 <ch2>
                if (ch1_min != None) and (ch2_max != None):
                        Imin = self.c1*ch1_min - self.c2*ch2_max                          # Imin = c1 ch1_min - c2 ch2_max
                if (ch1_var != None) and (ch2_var != None):
                        sigmaI = self.c1*np.sqrt(ch1_var) - self.c2*np.sqrt(ch2_var)      # sigma = c1 sigma_1 - c2 sigma_2
                self.myLCD_Filament.Update_U_I(voltage, current)
                self.myLCD_Anode.Display(Ua, Ia,Imin, sigmaI)

        def updatePlot(self):
                amount = 200
                x_label = self.PlotXAxis_ComboBox.currentText()
                y_label = self.PlotYAxis_ComboBox.currentText()
                Y_x = self.myData[[x_label, y_label]].dropna()
                self.Y_x_plot.update_plot(Y_x[x_label][max(0, Y_x.shape[0] - amount):Y_x.shape[0]],
                                                Y_x[y_label][max(0, Y_x.shape[0] - amount):Y_x.shape[0]])
                self.Y_x_plot.setAxisLabel(x_label, y_label)
                return

        def onCloseEvent(self):
                print("Disconnecting from all devices")
                try:
                        self.CommandTable.interruptTableExecution()
                except Exception as e:
                        print("CommandTable ", e)
                try:
                        if self.timer:
                                self.timer.stop()
                        self.myKorad.Set_v_i(v = 0,i = 0)
                except Exception as e:
                        print(e)

        def onTableFinish(self):
                print("Table execution finished")
                self.FinishMeasurements()

        def FinishMeasurements(self):
            self.myKorad.FinishExperiment()
            self.myLcardIF.myLcardDevice.removeListener()



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
    MainWindow.show()
    sys.exit(app.exec_())
