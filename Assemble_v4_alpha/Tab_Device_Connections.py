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

#------------------------ Device imports ------------------------------------
import Device_Korad as DKorad
import GUI_Korad_Connection
import Lcard_EmptyDevice



class TabDeviceConnections(object):
        def __init__(self):
                # devices with hardcoded default ini:
                self.myLcard_Device = Lcard_EmptyDevice.LcardE2010B_EmptyDevice("LcardE2010B.ini")
                self.myKorad_Device = DKorad.Korad("Korad.ini")

        def __del__(self):
                self.myLcard_Device.disconnectFromPhysicalDevice()
                self.myKorad_Device.DisconnectFromPhysicalDevice()

        def setupUi(self):
                self.centralwidget = QtWidgets.QWidget()
                
                # Korad : Connect Button
                self.QpButton_connectKorad = QtWidgets.QPushButton(self.centralwidget)
                self.QpButton_connectKorad.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.QpButton_connectKorad.setText("Connect Korad")
                self.QpButton_connectKorad.clicked.connect(self.onPushConnectKorad)
                # Korad : Config filename choice
                self.QLabel_FilenameKorad_ini = QtWidgets.QLabel("Korad.ini filename:", self.centralwidget)
                self.QLabel_FilenameKorad_ini.setStyleSheet("font: 75 15pt \"Tahoma\";")
                self.QLineEdit_FilenameKorad_ini = QtWidgets.QLineEdit(parent = self.centralwidget)
                self.QLineEdit_FilenameKorad_ini.setStyleSheet("font: 75 12pt \"Tahoma\";")
                self.QLineEdit_FilenameKorad_ini.setText("Korad.ini")
                self.QLineEdit_FilenameKorad_ini.setMaximumHeight(200)
                # Korad : Start - Stop Button
                self.QpButton_StartStopKorad = QtWidgets.QPushButton(self.centralwidget)
                self.QpButton_StartStopKorad.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.QpButton_StartStopKorad.setText("Start Korad")
                self.QpButton_StartStopKorad.clicked.connect(self.onPushStartStopKorad)
                # Korad : Set_I 
                self.QpButton_Korad_Set_I = QtWidgets.QPushButton(self.centralwidget)
                self.QpButton_Korad_Set_I.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.QpButton_Korad_Set_I.setText("Set I")
                self.QLineEdit_Korad_Set_I = QtWidgets.QLineEdit(parent = self.centralwidget)
                self.QLineEdit_Korad_Set_I.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.QpButton_Korad_Set_I.clicked.connect(self.onPushKoradSetI)
                # Korad : Set_U
                self.QpButton_Korad_Set_U = QtWidgets.QPushButton(self.centralwidget)
                self.QpButton_Korad_Set_U.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.QpButton_Korad_Set_U.setText("Set U")
                self.QLineEdit_Korad_Set_U = QtWidgets.QLineEdit(parent = self.centralwidget)
                self.QLineEdit_Korad_Set_U.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.QpButton_Korad_Set_U.clicked.connect(self.onPushKoradSetU)
                # Korad : Layout
                self.QLayout_Korad = QtWidgets.QVBoxLayout()
                self.QLayout_Korad.addWidget(self.QLabel_FilenameKorad_ini)
                self.QLayout_Korad.addWidget(self.QLineEdit_FilenameKorad_ini)
                self.QLayout_Korad.addWidget(self.QpButton_connectKorad)
                self.QLayout_Korad.addWidget(self.QpButton_StartStopKorad)
                QL_set_U_I = QtWidgets.QGridLayout()
                QL_set_U_I.addWidget(self.QpButton_Korad_Set_I, 0, 0)
                QL_set_U_I.addWidget(self.QLineEdit_Korad_Set_I, 0, 1)
                QL_set_U_I.addWidget(self.QpButton_Korad_Set_U, 1, 0)
                QL_set_U_I.addWidget(self.QLineEdit_Korad_Set_U, 1, 1)
                self.QLayout_Korad.addLayout(QL_set_U_I)
                # Korad : first update
                self.updateKoradGUI()
                
                # Lcard : Connect Button
                self.QpButton_connectLcard = QtWidgets.QPushButton(self.centralwidget)
                self.QpButton_connectLcard.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.QpButton_connectLcard.setText("Connect Lcard")
                self.QpButton_connectLcard.clicked.connect(self.onPushConnectLcard)
                # Lcard : Start Stop Button
                self.QpButton_StartStopLcard = QtWidgets.QPushButton(self.centralwidget)
                self.QpButton_StartStopLcard.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.QpButton_StartStopLcard.setText("Start Lcard")
                self.QpButton_StartStopLcard.clicked.connect(self.onPushStartStopLcard)
                # Lcard : Config filename choice
                self.QLabel_FilenameLcard_ini = QtWidgets.QLabel("Lcard.ini filename:", self.centralwidget)
                self.QLabel_FilenameLcard_ini.setStyleSheet("font: 75 15pt \"Tahoma\";")
                self.QLabel_FilenameLcard_ini.setMaximumHeight(50)
                self.QLineEdit_FilenameLcard_ini = QtWidgets.QLineEdit(parent = self.centralwidget)
                self.QLineEdit_FilenameLcard_ini.setStyleSheet("font: 75 12pt \"Tahoma\";")
                self.QLineEdit_FilenameLcard_ini.setText("LcardE2010B.ini")
                self.QLineEdit_FilenameLcard_ini.setMaximumHeight(50)
                # Lcard : Layout
                self.QLayout_Lcard = QtWidgets.QVBoxLayout()
                self.QLayout_Lcard.addWidget(self.QLabel_FilenameLcard_ini)
                self.QLayout_Lcard.addWidget(self.QLineEdit_FilenameLcard_ini)
                self.QLayout_Lcard.addWidget(self.QpButton_connectLcard)
                self.QLayout_Lcard.addWidget(self.QpButton_StartStopLcard)
                # Lcard : first update
                self.updateLcardGUI()
                
                # Layout
                self.QLayout_General = QtWidgets.QHBoxLayout()
                self.QLayout_General.addLayout(self.QLayout_Lcard)
                self.QLayout_General.addLayout(self.QLayout_Korad)
                self.centralwidget.setLayout(self.QLayout_General)

                return self.centralwidget

        def onCloseEvent(self):
                print("Disconnecting from all devices")
                self.disconnectKorad()
                self.disconnectLcard()
        
        # ------------ Korad --------------------
        def onPushConnectKorad(self):
            if self.myKorad_Device.IsConnected:
                self.disconnectKorad()
            else:
                self.connectKorad()
            return

        def onPushStartStopKorad(self):
            print("onPushStartStopKorad call", self.myKorad_Device.IsActiveMeasurements)
            if self.myKorad_Device.IsActiveMeasurements:
                self.stopKorad()
            else:
                self.startKorad()
            return

        def onPushKoradSetI(self):
            self.myKorad_Device.set_uncheckedI(self.QLineEdit_Korad_Set_I.text())

        def onPushKoradSetU(self):
            self.myKorad_Device.set_uncheckedU(self.QLineEdit_Korad_Set_U.text())

        def updateKoradGUI(self):
            self.QLineEdit_FilenameKorad_ini.setEnabled(not(self.myKorad_Device.IsConnected))
            self.QpButton_connectKorad.setEnabled(not(self.myKorad_Device.IsActiveMeasurements))
            self.QpButton_StartStopKorad.setEnabled(self.myKorad_Device.IsConnected)
            self.QpButton_Korad_Set_I.setEnabled(self.myKorad_Device.IsActiveMeasurements)
            self.QpButton_Korad_Set_U.setEnabled(self.myKorad_Device.IsActiveMeasurements)

            if self.myKorad_Device.IsConnected:
                self.QpButton_connectKorad.setText("Disconnect Korad")
            else:
                self.QpButton_connectKorad.setText("Connect Korad")

            if self.myKorad_Device.IsActiveMeasurements:
                self.QpButton_StartStopKorad.setText("Stop Korad")
            else:
                self.QpButton_StartStopKorad.setText("Start Korad")

        def connectKorad(self):
            self.myKorad_Device.DisconnectFromPhysicalDevice()
            self.myKorad_Device.ConfigFilename = self.QLineEdit_FilenameKorad_ini.text()
            self.myKorad_Device.ConnectToPhysicalDevice()
            self.updateKoradGUI()

        def disconnectKorad(self):
            self.myKorad_Device.DisconnectFromPhysicalDevice()
            self.updateKoradGUI()

        def startKorad(self):
            print("DeviceConnections.startKorad call")
            self.myKorad_Device.StartExperiment()
            self.updateKoradGUI()

        def stopKorad(self):
            self.myKorad_Device.FinishExperiment()
            self.updateKoradGUI()

        # ------------ Lcard --------------------
        def onPushConnectLcard(self):
            if self.myLcard_Device.IsConnected:
                self.disconnectLcard()
            else:
                self.connectLcard()
            return

        def onPushStartStopLcard(self):
            try:
                    if self.QpButton_StartStopLcard.text() == "Start Lcard":
                            self.myLcard_Device.addListener()
                    else:
                            self.myLcard_Device.finishMeasurements()
                    self.updateLcardGUI()
            except Exception as e:
                    print(e)

        def connectLcard(self):
            self.myLcard_Device.disconnectFromPhysicalDevice()
            self.myLcard_Device.ConfigFilename = self.QLineEdit_FilenameLcard_ini.text()
            self.myLcard_Device.connectToPhysicalDevice()
            self.updateLcardGUI()
        
        def disconnectLcard(self):
            self.myLcard_Device.disconnectFromPhysicalDevice()
            self.updateLcardGUI()

        def updateLcardGUI(self):
            self.QLineEdit_FilenameLcard_ini.setEnabled(not(self.myLcard_Device.IsConnected))
            self.QpButton_connectLcard.setEnabled(not(self.myLcard_Device.IsActiveMeasurements))
            self.QpButton_StartStopLcard.setEnabled(self.myLcard_Device.IsConnected)

            if self.myLcard_Device.IsConnected:
                self.QpButton_connectLcard.setText("Disconnect Lcard")
            else:
                self.QpButton_connectLcard.setText("Connect Lcard")

            if self.myLcard_Device.IsActiveMeasurements:
                    self.QpButton_StartStopLcard.setText("Stop Lcard")
            else:
                    self.QpButton_StartStopLcard.setText("Start Lcard")
                


def test():
    print("TabDeviceConnections test")
    import sys 
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = MainWindow_withCloseEvent()
    ui = TabDeviceConnections()
    centralwidget = ui.setupUi()
    MainWindow.setCentralWidget(centralwidget)
    MainWindow.CloseEventListeners.append(ui.onCloseEvent)
    MainWindow.show()
    app.exec_()

    print(ui.myKorad_Device.ConfigFilename)
    print(ui.myLcard_Device.ConfigFilename)

if __name__ == "__main__":
    try:
        test()
        print(">> success")
    except Exception as e:
        print(e)
        a = input()
    
