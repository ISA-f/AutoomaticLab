from PyQt5 import QtCore, QtGui, QtWidgets
from Device_LcardE2010B_PeriodicCall import LcardE2010B_PeriodicCall
import pandas as pd
import time

class LcardE2010B_Interface(object):
        def __init__(self, centralwidget):
                self.centralwidget = centralwidget
                self._translate = QtCore.QCoreApplication.translate
                self.myLcard = None
        
        def SetupUI(self):
                self.pushButton_Connect_Disconnect_Lcard = QtWidgets.QPushButton(self.centralwidget)
                self.pushButton_Connect_Disconnect_Lcard.setGeometry(QtCore.QRect(900, 40, 300, 51))
                self.pushButton_Connect_Disconnect_Lcard.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.pushButton_Connect_Disconnect_Lcard.setObjectName("Connect_Disconnect Lcard")
                self.pushButton_Connect_Disconnect_Lcard.setText(self._translate("MainWindow", "Connect Lcard"))
                
                self.pushButton_Start_Finish_Measurements = QtWidgets.QPushButton(self.centralwidget)
                self.pushButton_Start_Finish_Measurements.setGeometry(QtCore.QRect(700, 440, 151, 51))
                self.pushButton_Start_Finish_Measurements.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.pushButton_Start_Finish_Measurements.setObjectName("Start_Finish_Measurements Lcard")
                self.pushButton_Start_Finish_Measurements.setText(self._translate("MainWindow", "Start Lcard Measurements"))
                self.pushButton_Start_Finish_Measurements.setEnabled(False)
                
                vbox = QtWidgets.QVBoxLayout()
                vbox.addWidget(self.pushButton_Connect_Disconnect_Lcard)
                vbox.addWidget(self.pushButton_Start_Finish_Measurements)
                self.centralwidget.setLayout(vbox)

                self.pushButton_Connect_Disconnect_Lcard.clicked.connect(self.Push_Connect_Disconnect_Button)
                #self.pushButton_Start_Finish_Measurements.clicked.connect(self.Set_I)
                

        def Push_Connect_Disconnect_Button(self):
                self.Connect_Disconnect(self.pushButton_Connect_Disconnect_Lcard.text() == "Connect Lcard")

        def Connect_Disconnect(self, connect: bool):
                if connect:
                        if not(self.myLcard):
                                self.myLcard = LcardE2010B_PeriodicCall('LcardE2010B.ini')
                        try:
                                self.myLcard.ConnectToPhysicalDevice()
                                self.pushButton_Connect_Disconnect_Lcard.setText(self._translate("MainWindow", "Disconnect Lcard"))
                                self.pushButton_Start_Finish_Measurements.setEnabled(True)
                        except Exception as e:
                                print(e)
                else:

                        print(self.myLcard)
                        if (self.myLcard):
                                self.myLcard.DisconnectFromPhysicalDevice()
                        self.pushButton_Connect_Disconnect_Lcard.setText(self._translate("MainWindow", "Connect Lcard"))
                        self.pushButton_Start_Finish_Measurements.setEnabled(False)
                        #self.myLcard.TakeMeasurements()
                

        def Start_Finish_Measurements(self, start: bool):
                if not(self.myLcard):
                        return
                if start:
                        filename = "LcardE2010B_" + str(time.time()) + ".log"
                        self.myLcard.StartMeasurements(filename)
                        self.pushButton_Start_Finish_Measurements.setText(self._translate("MainWindow", "Finish Measurements"))
                        
                else:
                        self.myLcard.FinishMeasurements()
                        self.pushButton_Start_Finish_Measurements.setText(self._translate("MainWindow", "Start Measurements"))
                        
        
        def TakeMeasurements(self, requested_buffer_size):
            print(self.myLcard.IsActiveMeasurements)
            if not(self.myLcard):
                        print("Tried to take measurements with Lcard disconnected")
                        return pd.Series([None for i in range(10)])
            if not(self.myLcard.IsActiveMeasurements):
                    print("Have not started Lcard measurements")
                    return
            return self.myLcard.TakeMeasurements(requested_buffer_size)
