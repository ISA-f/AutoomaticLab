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
                self.QpButton_connectKorad.clicked.connect(self.onPushKorad)
                # Korad : Config filename choice
                self.QLabel_FilenameKorad_ini = QtWidgets.QLabel("Korad.ini filename:", self.centralwidget)
                self.QLabel_FilenameKorad_ini.setStyleSheet("font: 75 15pt \"Tahoma\";")
                self.QLineEdit_FilenameKorad_ini = QtWidgets.QLineEdit(parent = self.centralwidget)
                self.QLineEdit_FilenameKorad_ini.setStyleSheet("font: 75 12pt \"Tahoma\";")
                self.QLineEdit_FilenameKorad_ini.setText("Korad.ini")
                # Korad : Set_I 
                self.pushButton_Set_I = QtWidgets.QPushButton(self.centralwidget)
                self.pushButton_Set_I.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.pushButton_Set_I.setText("Set I")
                self.pushButton_Set_I.setEnabled(False)
                self.QLineEdit_Set_I = QtWidgets.QLineEdit(parent = self.centralwidget)
                self.QLineEdit_Set_I.setStyleSheet("font: 75 18pt \"Tahoma\";")
                # Korad : Set_U
                self.pushButton_Set_U = QtWidgets.QPushButton(self.centralwidget)
                self.pushButton_Set_U.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.pushButton_Set_U.setText("Set U")
                self.pushButton_Set_U.setEnabled(False)
                self.QLineEdit_Set_U = QtWidgets.QLineEdit(parent = self.centralwidget)
                self.QLineEdit_Set_U.setStyleSheet("font: 75 18pt \"Tahoma\";")
                
                # Korad : Layout
                self.QLayout_Korad = QtWidgets.QVBoxLayout()
                self.QLayout_Korad.addWidget(self.QLabel_FilenameKorad_ini)
                self.QLayout_Korad.addWidget(self.QLineEdit_FilenameKorad_ini)
                self.QLayout_Korad.addWidget(self.QpButton_connectKorad)
                hbox_I = QtWidgets.QHBoxLayout()
                hbox_I.addWidget(self.pushButton_Set_I)
                hbox_I.addWidget(self.QLineEdit_Set_I)
                
                # Lcard : Connect Button
                self.QpButton_connectLcard = QtWidgets.QPushButton(self.centralwidget)
                self.QpButton_connectLcard.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.QpButton_connectLcard.setText("Connect Lcard")
                self.QpButton_connectLcard.clicked.connect(self.onPushLcard)
                # Lcard : Start Stop Button
                self.QpButton_StartStopLcard = QtWidgets.QPushButton(self.centralwidget)
                self.QpButton_StartStopLcard.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.QpButton_StartStopLcard.setText("Start Lcard")
                self.QpButton_StartStopLcard.setEnabled(False)
                self.QpButton_StartStopLcard.clicked.connect(self.onPushStartStopLcard)
                # Lcard : Config filename choice
                self.QLabel_FilenameLcard_ini = QtWidgets.QLabel("Lcard.ini filename:", self.centralwidget)
                self.QLabel_FilenameLcard_ini.setStyleSheet("font: 75 15pt \"Tahoma\";")
                self.QLineEdit_FilenameLcard_ini = QtWidgets.QLineEdit(parent = self.centralwidget)
                self.QLineEdit_FilenameLcard_ini.setStyleSheet("font: 75 12pt \"Tahoma\";")
                self.QLineEdit_FilenameLcard_ini.setText("LcardE2010B.ini")
                # Lcard : Layout
                self.QLayout_Lcard = QtWidgets.QVBoxLayout()
                self.QLayout_Lcard.addWidget(self.QLabel_FilenameLcard_ini)
                self.QLayout_Lcard.addWidget(self.QLineEdit_FilenameLcard_ini)
                self.QLayout_Lcard.addWidget(self.QpButton_connectLcard)
                self.QLayout_Lcard.addWidget(self.QpButton_StartStopLcard)
                
                # Layout
                self.QLayout_General = QtWidgets.QHBoxLayout()
                self.QLayout_General.addLayout(self.QLayout_Lcard)
                self.QLayout_General.addLayout(self.QLayout_Korad)
                self.centralwidget.setLayout(self.QLayout_General)

                self.centralwidget.setMaximumSize(600, 200)
                return self.centralwidget

        def onCloseEvent(self):
                print("Disconnecting from all devices")
                self.disconnectKorad()
                self.disconnectLcard()
        
        # ------------ Korad --------------------
        def onPushKorad(self):
            if self.getIsKoradConnected():
                self.disconnectKorad()
            else:
                self.connectKorad()
            return

        def getIsKoradConnected(self):
            return not(self.myKorad_Device.ser is None)
        
        def updateIsKoradConnected(self):
            self.QLineEdit_FilenameKorad_ini.setEnabled(not(self.getIsKoradConnected()))
            if self.getIsKoradConnected():
                self.QpButton_connectKorad.setText("Disconnect Korad")
            else:
                self.QpButton_connectKorad.setText("Connect Korad")

        def connectKorad(self):
            self.disconnectKorad()
            try:
                self.myKorad_Device.ConfigFilename = self.QLineEdit_FilenameKorad_ini.text()
                self.myKorad_Device.ConnectToPhysicalDevice()
            except Exception as e:
                print(e)
            self.updateIsKoradConnected()

        def disconnectKorad(self):
            self.myKorad_Device.DisconnectFromPhysicalDevice()
            self.updateIsKoradConnected()

        # ------------ Lcard --------------------
        def onPushLcard(self):
            if self.getIsLcardConnected():
                self.disconnectLcard()
            else:
                self.connectLcard()
            return

        def onPushStartStopLcard(self):
            if self.QpButton_StartStopLcard.text() == "Start Lcard":
                    self.myLcard_Device.startMeasurements()
            else:
                    self.myLcard_Device.finishMeasurements()
            self.updateIsLcardActiveMeasurements()

        def connectLcard(self):
            self.disconnectLcard()
            try:
                self.myLcard_Device.ConfigFilename = self.QLineEdit_FilenameLcard_ini.text()
                self.myLcard_Device.connectToPhysicalDevice()
                self.myLcard_Device.loadConfiguration()
            except Exception as e:
                print(e)
                self.myLcard_Device.IsConnected = False
            self.updateIsLcardConnected()
        
        def disconnectLcard(self):
            self.myLcard_Device.disconnectFromPhysicalDevice()
            self.updateIsLcardConnected()

        def getIsLcardConnected(self):
            return self.myLcard_Device.IsConnected

        def getIsLcardActiveMeasurements(self):
            return self.myLcard_Device.IsActiveMeasurements

        def updateIsLcardConnected(self):
            self.updateIsLcardActiveMeasurements()
            self.QLineEdit_FilenameLcard_ini.setEnabled(not(self.getIsLcardConnected()))
            if self.getIsLcardConnected():
                self.QpButton_connectLcard.setText("Disconnect Lcard")
            else:
                self.QpButton_connectLcard.setText("Connect Lcard")

        def updateIsLcardActiveMeasurements(self):
            self.QpButton_StartStopLcard.setEnabled(self.getIsLcardConnected())
            if self.getIsLcardActiveMeasurements():
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
    
