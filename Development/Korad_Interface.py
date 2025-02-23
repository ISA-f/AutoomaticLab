from PyQt5 import QtCore, QtGui, QtWidgets
from Device_Korad import Korad
import pandas as pd

class Korad_Interface(object):
        def __init__(self, centralwidget):
                self.centralwidget = centralwidget
                self._translate = QtCore.QCoreApplication.translate
                self.myKorad = None
        
        def SetupUI(self):
                self.pushButton_Set_I = QtWidgets.QPushButton(self.centralwidget)
                self.pushButton_Set_I.setGeometry(QtCore.QRect(700, 140, 151, 51))
                self.pushButton_Set_I.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.pushButton_Set_I.setObjectName("Set I")
                self.pushButton_Set_I.setText(self._translate("MainWindow", "Set I"))
                self.pushButton_Set_I.setEnabled(False)
                self.QLineEdit_Set_I = QtWidgets.QLineEdit(parent = self.centralwidget)
                self.QLineEdit_Set_I.setGeometry(QtCore.QRect(900, 140, 151, 51))
                self.QLineEdit_Set_I.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.QLineEdit_Set_I.setObjectName("Set I Line")
                hbox_I = QtWidgets.QHBoxLayout()
                hbox_I.addWidget(self.pushButton_Set_I)
                hbox_I.addWidget(self.QLineEdit_Set_I)
                
                self.pushButton_Connect_Disconnect_Korad = QtWidgets.QPushButton(self.centralwidget)
                self.pushButton_Connect_Disconnect_Korad.setGeometry(QtCore.QRect(900, 40, 300, 51))
                self.pushButton_Connect_Disconnect_Korad.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.pushButton_Connect_Disconnect_Korad.setObjectName("Connect_Disconnect Korad")
                self.pushButton_Connect_Disconnect_Korad.setText(self._translate("MainWindow", "Connect Korad"))
                
                self.pushButton_Set_U = QtWidgets.QPushButton(self.centralwidget)
                self.pushButton_Set_U.setGeometry(QtCore.QRect(700, 440, 151, 51))
                self.pushButton_Set_U.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.pushButton_Set_U.setObjectName("Set U")
                self.pushButton_Set_U.setText(self._translate("MainWindow", "Set U"))
                self.pushButton_Set_U.setEnabled(False)
                self.QLineEdit_Set_U = QtWidgets.QLineEdit(parent = self.centralwidget)
                self.QLineEdit_Set_U.setGeometry(QtCore.QRect(900, 140, 151, 51))
                self.QLineEdit_Set_U.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.QLineEdit_Set_U.setObjectName("Set U Line")
                hbox_U = QtWidgets.QHBoxLayout()
                hbox_U.addWidget(self.pushButton_Set_U)
                hbox_U.addWidget(self.QLineEdit_Set_U)
                
                vbox = QtWidgets.QVBoxLayout()
                vbox.addWidget(self.pushButton_Connect_Disconnect_Korad)
                vbox.addLayout(hbox_I)
                vbox.addLayout(hbox_U)
                self.centralwidget.setLayout(vbox)

                self.pushButton_Connect_Disconnect_Korad.clicked.connect(self.Push_Connect_Disconnect_Button)
                self.pushButton_Set_I.clicked.connect(self.Set_I_QLineEdit)
                self.pushButton_Set_U.clicked.connect(self.Set_U_QLineEdit)

        def Push_Connect_Disconnect_Button(self):
                self.Connect_Disconnect(self.pushButton_Connect_Disconnect_Korad.text() == "Connect Korad")

        def Connect_Disconnect(self, connect: bool):
                if connect:
                        print("Try connect Korad")
                        if not(self.myKorad):
                                self.myKorad = Korad('Korad.ini')
                        try:
                                self.myKorad.ConnectToPhysicalDevice()
                                self.pushButton_Connect_Disconnect_Korad.setText(self._translate("MainWindow", "Disconnect Korad"))
                        except Exception as e:
                                print(e)
                elif self.myKorad and self.myKorad.ser:
                        self.myKorad.DisconnectFromPhysicalDevice()
                        self.pushButton_Connect_Disconnect_Korad.setText(self._translate("MainWindow", "Connect Korad"))

                # updating GUI
                if self.myKorad and self.myKorad.ser:
                        self.pushButton_Set_I.setEnabled(bool(self.myKorad.ser))
                        self.pushButton_Set_U.setEnabled(bool(self.myKorad.ser))

        def Start_Finish(self, start: bool):
                if not(self.myKorad):
                        return
                if start:
                        try:
                                self.StartExperiment()
                                #self.pushButton_Connect_Disconnect_Korad.setText(self._translate("MainWindow", "Disconnect Korad"))
                        except Exception as e:
                                print(e)
                                a = input()
                else:
                        self.FinishExperiment()

        def Set_I_QLineEdit(self):
                self.Set_I(self.QLineEdit_Set_I.text())
                return

        def Set_U_QLineEdit(self):
                self.Set_U(self.QLineEdit_Set_U.text())
                return
        
        def Set_I(self, value):
                s = None
                try:
                        s = float(value)
                except Exception as e:
                        pass
                if(s):
                        self.myKorad.Set_v_i(i = s)
                return

        def Set_U(self, value):
                s = None
                try:
                        s = float(value)
                except Exception as e:
                        pass
                if(s):
                        self.myKorad.Set_v_i(v = s)
                return

        def TakeMeasurements(self):
            if self.myKorad and self.myKorad.ser :
                return self.myKorad.TakeMeasurements()
            return pd.Series([None, None, None])

        def StartExperiment(self):
                if self.myKorad and self.myKorad.ser :
                        self.myKorad.StartExperiment()

        def FinishExperiment(self):
                if self.myKorad:
                        self.myKorad.FinishExperiment()
