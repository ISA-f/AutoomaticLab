#------------------------ general imports -----------------------------------

import configparser

#------------------------ Qt and GUI imports --------------------------------
from PyQt5 import QtCore, QtGui, QtWidgets

#------------------------ Device imports ------------------------------------
import Device_Korad as DKorad


class GUI_Korad_Connection(object):
        def __init__(self):
            # device with hardcoded default ini:
            self.myKorad_Device = DKorad.Korad("Korad.ini")

        def __del__(self):
            self.myKorad_Device.DisconnectFromPhysicalDevice()

        def setupUi(self):
            self.centralwidget = QtWidgets.QWidget()
            # Connect Button
            self.QpButton_connect = QtWidgets.QPushButton(self.centralwidget)
            self.QpButton_connect.setStyleSheet("font: 75 18pt \"Tahoma\";")
            self.QpButton_connect.setText("Connect Korad")
            self.QpButton_connect.clicked.connect(self.onPushConnect)
            # Config filename choice
            self.QLabel_Filename_ini = QtWidgets.QLabel("Korad.ini filename:", self.centralwidget)
            self.QLabel_Filename_ini.setStyleSheet("font: 75 15pt \"Tahoma\";")
            self.QLineEdit_Filename_ini = QtWidgets.QLineEdit(parent = self.centralwidget)
            self.QLineEdit_Filename_ini.setStyleSheet("font: 75 12pt \"Tahoma\";")
            self.QLineEdit_Filename_ini.setText("Korad.ini")
            # Button Set_I 
            self.QpButton_Set_I = QtWidgets.QPushButton(self.centralwidget)
            self.QpButton_Set_I.setStyleSheet("font: 75 18pt \"Tahoma\";")
            self.QpButton_Set_I.setText("Set I")
            self.QpButton_Set_I.setEnabled(False)
            self.QLineEdit_Set_I = QtWidgets.QLineEdit(parent = self.centralwidget)
            self.QLineEdit_Set_I.setStyleSheet("font: 75 18pt \"Tahoma\";")
            # Button Set_U
            self.QpButton_Set_U = QtWidgets.QPushButton(self.centralwidget)
            self.QpButton_Set_U.setStyleSheet("font: 75 18pt \"Tahoma\";")
            self.QpButton_Set_U.setText("Set U")
            self.QpButton_Set_U.setEnabled(False)
            self.QLineEdit_Set_U = QtWidgets.QLineEdit(parent = self.centralwidget)
            self.QLineEdit_Set_U.setStyleSheet("font: 75 18pt \"Tahoma\";")
                
            # Layout
            self.QLayout_General = QtWidgets.QVBoxLayout()
            self.QLayout_General.addWidget(self.QLabel_Filename_ini)
            self.QLayout_General.addWidget(self.QLineEdit_Filename_ini)
            self.QLayout_General.addWidget(self.QpButton_connectKorad)
            hbox_I = QtWidgets.QHBoxLayout()
            hbox_I.addWidget(self.pushButton_Set_I)
            hbox_I.addWidget(self.QLineEdit_Set_I)
               
            # Lcard : Start Stop Button
            self.QpButton_StartStopLcard = QtWidgets.QPushButton(self.centralwidget)
            self.QpButton_StartStopLcard.setStyleSheet("font: 75 18pt \"Tahoma\";")
            self.QpButton_StartStopLcard.setText("Start Lcard")
            self.QpButton_StartStopLcard.setEnabled(False)
            self.QpButton_StartStopLcard.clicked.connect(self.onPushStartStopLcard)
                
            self.centralwidget.setLayout(self.QLayout_General)
            self.centralwidget.setMaximumSize(600, 200)
            return self.centralwidget
        
        def onPushConnect(self):
            if self.getIsKoradConnected():
                self.disconnectKorad()
            else:
                self.connectKorad()
            return

        def getIsConnected(self):
            return not(self.myKorad_Device.ser is None)
        
        def updateIsKoradConnected(self):
            self.QLineEdit_FilenameKorad_ini.setEnabled(not(self.getIsConnected()))
            if self.getIsConnected():
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

        def onCloseEvent(self):
            self.disconnectKorad()


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
    
