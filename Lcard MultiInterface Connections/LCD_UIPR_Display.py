from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtCore import QTimer
import time
import pandas as pd

from Updatable_QTCanvas import PyplotWidget

from Device_Korad import Korad
from Korad_Interface import Korad_Interface
from Device_LcardE440 import LcardE440_Autoread
#from Device_LcardE2010B import LcardE2010B_Autoread
from Device_LcardE2010B_PeriodicCall import LcardE2010B_PeriodicCall
from LCD_Filament import LCD_Filament


class FilamentAnode(object):

        def __init__(self, my_device = 'Device_LcardE440'):
                self.my_device = my_device # Пока что подефолту, программа не тестировалась на 2010
                self.my_confog_lcard = None
                self.myData = pd.DataFrame(columns = ['Korad_time', 'Korad_U', 'Korad_I',
                                                      'Lcard_comp_time', 'Lcard_buffer_size',
                                                      'Lcard_Ch0_mean','Lcard_Ch0_var', 'Lcard_Ch0_min', 'Lcard_Ch0_max',
                                                      'Lcard_Ch1_mean','Lcard_Ch1_var', 'Lcard_Ch1_min', 'Lcard_Ch1_max'])
                
                if my_device == 'Device_LcardE440':
                        self.my_confog_lcard = "LcardE440.ini"
                        self.myLcard = LcardE440_Autoread(self.my_confog_lcard)
                elif my_device == 'Device_LcardE2010B':
                        self.my_confog_lcard = "LcardE2010B.ini"
                        self.myLcard = LcardE2010B_PeriodicCall(self.my_confog_lcard)
                else:
                        raise NameError("invalid Lcard type")



        def setupUi(self, MainWindow):
                MainWindow.setObjectName("MainWindow")
                MainWindow.resize(1200, 900)
                
                self.centralwidget = QtWidgets.QWidget(MainWindow)
                self.centralwidget.setObjectName("centralwidget")
                
                self.LCD_widget = QtWidgets.QWidget(self.centralwidget)
                self.LCD_widget.setGeometry(QtCore.QRect(500, 100, 500, 500))
                self.myLCD_Filament = LCD_Filament(self.LCD_widget)
                self.myLCD_Filament.SetupUI(self.LCD_widget)
                """
                self.label_dark_gray_anode = QtWidgets.QLabel(self.centralwidget)
                self.label_dark_gray_anode.setGeometry(QtCore.QRect(400, 100, 241, 51))
                self.label_dark_gray_anode.setStyleSheet("background-color: rgb(134, 134, 134);")
                self.label_dark_gray_anode.setText("")
                self.label_dark_gray_anode.setObjectName("label_dark_gray_anode")
                self.label_mA_for_Ia_anode = QtWidgets.QLabel(self.centralwidget)
                self.label_mA_for_Ia_anode.setGeometry(QtCore.QRect(580, 220, 51, 31))
                self.label_mA_for_Ia_anode.setStyleSheet("font: 500 18pt \"Tahoma\";\n"
                                                         "color: rgb(231, 76, 76)")
                self.label_mA_for_Ia_anode.setObjectName("label_mA_for_Ia_anode")
                self.label_mA_for_sigmaI = QtWidgets.QLabel(self.centralwidget)
                self.label_mA_for_sigmaI.setGeometry(QtCore.QRect(580, 370, 51, 31))
                self.label_mA_for_sigmaI.setStyleSheet("font: 500 18pt \"Tahoma\";\n"
                                                       "color: rgb(231, 76, 76)")
                self.label_mA_for_sigmaI.setObjectName("label_mA_for_sigmaI")
                self.label_mA_for_Imin_anode = QtWidgets.QLabel(self.centralwidget)
                self.label_mA_for_Imin_anode.setGeometry(QtCore.QRect(580, 270, 81, 31))
                self.label_mA_for_Imin_anode.setStyleSheet("font: 500 18pt \"Tahoma\";\n"
                                                           "color: rgb(231, 76, 76)")
                self.label_mA_for_Imin_anode.setObjectName("label_mA_for_Imin_anode")
                self.label_anode = QtWidgets.QLabel(self.centralwidget)
                self.label_anode.setGeometry(QtCore.QRect(483, 100, 91, 51))
                self.label_anode.setStyleSheet("font: 500 18pt \"Tahoma\";\n"
                                               "color:rgb(255, 255, 255)")
                self.label_anode.setObjectName("label_anode")
                self.label_Ia_anode = QtWidgets.QLabel(self.centralwidget)
                self.label_Ia_anode.setGeometry(QtCore.QRect(416, 220, 21, 31))
                self.label_Ia_anode.setStyleSheet("font: 500 18pt \"Tahoma\";\n"
                                                  "color: rgb(231, 76, 76)")
                self.label_Ia_anode.setObjectName("label_Ia_anode")
                self.line_anode = QtWidgets.QFrame(self.centralwidget)
                self.line_anode.setGeometry(QtCore.QRect(400, 142, 241, 20))
                self.line_anode.setFrameShape(QtWidgets.QFrame.HLine)
                self.line_anode.setFrameShadow(QtWidgets.QFrame.Sunken)
                self.line_anode.setObjectName("line_anode")
                self.label_sigmaI_anode = QtWidgets.QLabel(self.centralwidget)
                self.label_sigmaI_anode.setGeometry(QtCore.QRect(415, 370, 21, 31))
                self.label_sigmaI_anode.setStyleSheet("font: 500 18pt \"Tahoma\";\n"
                                                      "color: rgb(231, 76, 76)")
                self.label_sigmaI_anode.setObjectName("label_sigmaI_anode")
                self.label_B_for_Ua_anode = QtWidgets.QLabel(self.centralwidget)
                self.label_B_for_Ua_anode.setGeometry(QtCore.QRect(580, 170, 21, 31))
                self.label_B_for_Ua_anode.setStyleSheet("font: 500 18pt \"Tahoma\";\n"
                                                        "color: rgb(231, 76, 76)")
                self.label_B_for_Ua_anode.setObjectName("label_B_for_Ua_anode")
                self.label_light_gray_anode = QtWidgets.QLabel(self.centralwidget)
                self.label_light_gray_anode.setGeometry(QtCore.QRect(400, 100, 241, 321))
                self.label_light_gray_anode.setStyleSheet("background-color: rgb(200, 200, 200);")
                self.label_light_gray_anode.setText("")
                self.label_light_gray_anode.setObjectName("label_light_gray_anode")
                self.label_Imax_anode = QtWidgets.QLabel(self.centralwidget)
                self.label_Imax_anode.setGeometry(QtCore.QRect(417, 320, 21, 31))
                self.label_Imax_anode.setStyleSheet("font: 500 18pt \"Tahoma\";\n"
                                                    "color: rgb(231, 76, 76)")
                self.label_Imax_anode.setObjectName("label_Imax_anode")
                self.label_mA_for_Imax_anode = QtWidgets.QLabel(self.centralwidget)
                self.label_mA_for_Imax_anode.setGeometry(QtCore.QRect(580, 320, 51, 31))
                self.label_mA_for_Imax_anode.setStyleSheet("font: 500 18pt \"Tahoma\";\n"
                                                           "color: rgb(231, 76, 76)")
                self.label_mA_for_Imax_anode.setObjectName("label_mA_for_Imax_anode")
                self.label_Imin_anode = QtWidgets.QLabel(self.centralwidget)
                self.label_Imin_anode.setGeometry(QtCore.QRect(417, 270, 21, 31))
                self.label_Imin_anode.setStyleSheet("font: 500 18pt \"Tahoma\";\n"
                                                    "color: rgb(231, 76, 76)")
                self.label_Imin_anode.setObjectName("label_Imin_anode")
                self.label_Ua_anode = QtWidgets.QLabel(self.centralwidget)
                self.label_Ua_anode.setGeometry(QtCore.QRect(411, 170, 31, 31))
                self.label_Ua_anode.setStyleSheet("font: 500 18pt \"Tahoma\";\n"
                                                  "color: rgb(231, 76, 76)")
                self.label_Ua_anode.setObjectName("label_Ua_anode")
                self.label_28 = QtWidgets.QLabel(self.centralwidget)
                self.label_28.setGeometry(QtCore.QRect(430, 230, 31, 31))
                self.label_28.setStyleSheet("font: 500 10pt \"Tahoma\";\n"
                                            "color: rgb(231, 76, 76)")
                self.label_28.setObjectName("label_28")
                self.label_a_for_U_anode = QtWidgets.QLabel(self.centralwidget)
                self.label_a_for_U_anode.setGeometry(QtCore.QRect(431, 180, 31, 31))
                self.label_a_for_U_anode.setStyleSheet("font: 500 10pt \"Tahoma\";\n"
                                                       "color: rgb(231, 76, 76)")
                self.label_a_for_U_anode.setObjectName("label_a_for_U_anode")
                self.label_30 = QtWidgets.QLabel(self.centralwidget)
                self.label_30.setGeometry(QtCore.QRect(430, 280, 41, 31))
                self.label_30.setStyleSheet("font: 500 10pt \"Tahoma\";\n"
                                            "color: rgb(231, 76, 76)")
                self.label_30.setObjectName("label_30")
                self.label_31 = QtWidgets.QLabel(self.centralwidget)
                self.label_31.setGeometry(QtCore.QRect(430, 330, 41, 31))
                self.label_31.setStyleSheet("font: 500 10pt \"Tahoma\";\n"
                                            "color: rgb(231, 76, 76)")
                self.label_31.setObjectName("label_31")
                self.label_32 = QtWidgets.QLabel(self.centralwidget)
                self.label_32.setGeometry(QtCore.QRect(433, 380, 16, 31))
                self.label_32.setStyleSheet("font: 500 10pt \"Tahoma\";\n"
                                            "color: rgb(231, 76, 76)")
                self.label_32.setObjectName("label_32")
                """
                """
                self.lcdnumber_sigmaI_anode = QtWidgets.QLCDNumber(self.centralwidget)
                self.lcdnumber_sigmaI_anode.setGeometry(QtCore.QRect(470, 370, 101, 31))
                self.lcdnumber_sigmaI_anode.setStyleSheet("background-color: rgb(59, 59, 59);\n"
                                                          "color: rgb(255, 0, 0);")
                self.lcdnumber_sigmaI_anode.setObjectName("lcdnumber_sigmaI_anode")
                self.lcdnumber_Ua_anode = QtWidgets.QLCDNumber(self.centralwidget)
                self.lcdnumber_Ua_anode.setGeometry(QtCore.QRect(470, 170, 101, 31))
                self.lcdnumber_Ua_anode.setStyleSheet("background-color: rgb(59, 59, 59);\n"
                                                      "color: rgb(255, 0, 0);")
                self.lcdnumber_Ua_anode.setObjectName("lcdnumber_Ua_anode")
                self.lcdnumber_Imin_anode = QtWidgets.QLCDNumber(self.centralwidget)
                self.lcdnumber_Imin_anode.setGeometry(QtCore.QRect(470, 270, 101, 31))
                self.lcdnumber_Imin_anode.setStyleSheet("background-color: rgb(59, 59, 59);\n"
                                                        "color: rgb(255, 0, 0);")
                self.lcdnumber_Imin_anode.setObjectName("lcdnumber_Imin_anode")
                self.lcdnumber_Imax_anode = QtWidgets.QLCDNumber(self.centralwidget)
                self.lcdnumber_Imax_anode.setGeometry(QtCore.QRect(470, 320, 101, 31))
                self.lcdnumber_Imax_anode.setStyleSheet("background-color: rgb(59, 59, 59);\n"
                                                        "color: rgb(255, 0, 0);")
                self.lcdnumber_Imax_anode.setObjectName("lcdnumber_Imax_anode")
                self.lcdnumber_Ia_anode = QtWidgets.QLCDNumber(self.centralwidget)
                self.lcdnumber_Ia_anode.setGeometry(QtCore.QRect(470, 220, 101, 31))
                self.lcdnumber_Ia_anode.setStyleSheet("background-color: rgb(59, 59, 59);\n"
                                                      "color: rgb(255, 0, 0);")
                self.lcdnumber_Ia_anode.setObjectName("lcdnumber_Ia_anode")
                """
                #self.label_light_gray_anode.raise_()

                #self.label_dark_gray_anode.raise_()
                #self.label_mA_for_Ia_anode.raise_()
                #self.label_mA_for_sigmaI.raise_()
                #self.label_mA_for_Imin_anode.raise_()
                #self.label_anode.raise_()
                #self.label_Ia_anode.raise_()
                #self.line_anode.raise_()
                #self.label_sigmaI_anode.raise_()
                #self.label_B_for_Ua_anode.raise_()
                #self.label_Imax_anode.raise_()
                #self.label_mA_for_Imax_anode.raise_()
                #self.label_Imin_anode.raise_()
                #self.label_Ua_anode.raise_()
                #self.label_28.raise_()
                #self.label_a_for_U_anode.raise_()
                #self.label_30.raise_()
                #self.label_31.raise_()
                #self.label_32.raise_()

                #self.lcdnumber_sigmaI_anode.raise_()
                #self.lcdnumber_Ua_anode.raise_()
                #self.lcdnumber_Imin_anode.raise_()
                #self.lcdnumber_Imax_anode.raise_()
                #self.lcdnumber_Ia_anode.raise_()
                MainWindow.setCentralWidget(self.centralwidget)
                self.statusbar = QtWidgets.QStatusBar(MainWindow)
                self.statusbar.setObjectName("statusbar")
                MainWindow.setStatusBar(self.statusbar)
                
                self.retranslateUi(MainWindow)
                QtCore.QMetaObject.connectSlotsByName(MainWindow)

        def retranslateUi(self, MainWindow):
                _translate = QtCore.QCoreApplication.translate
                MainWindow.setWindowTitle(_translate("MainWindow", "MainWindow"))
                
                self.label_filament.setText(_translate("MainWindow", "Накал"))
                self.label_I_filament.setText(_translate("MainWindow", "I"))
                self.label_A_filament.setText(_translate("MainWindow", "A"))
                self.label_B_filament.setText(_translate("MainWindow", "B"))
                self.label_U_filament.setText(_translate("MainWindow", "U"))
                self.label_mO_filament.setText(_translate("MainWindow", "mO"))
                self.label_R_filament.setText(_translate("MainWindow", "R"))
                self.label_W_filament.setText(_translate("MainWindow", "W"))
                self.label_P_filament.setText(_translate("MainWindow", "P"))
                self.label_K_filament.setText(_translate("MainWindow", "K"))
                self.label_T_filament.setText(_translate("MainWindow", "T"))
                #self.label_mA_for_Ia_anode.setText(_translate("MainWindow", "mA"))
                #self.label_mA_for_sigmaI.setText(_translate("MainWindow", "mA"))
                #self.label_mA_for_Imin_anode.setText(_translate("MainWindow", "mA"))
                #self.label_anode.setText(_translate("MainWindow", "Анод"))
                #self.label_Ia_anode.setText(_translate("MainWindow", "I"))
                #self.label_sigmaI_anode.setText(_translate("MainWindow", "o"))
                #self.label_B_for_Ua_anode.setText(_translate("MainWindow", "B"))
                #self.label_Imax_anode.setText(_translate("MainWindow", "I"))
                #self.label_mA_for_Imax_anode.setText(_translate("MainWindow", "mA"))
                #self.label_Imin_anode.setText(_translate("MainWindow", "I"))
                #self.label_Ua_anode.setText(_translate("MainWindow", "U"))
                #self.label_28.setText(_translate("MainWindow", "a"))
                #self.label_a_for_U_anode.setText(_translate("MainWindow", "a"))
                #self.label_30.setText(_translate("MainWindow", "min"))
                #self.label_31.setText(_translate("MainWindow", "max"))
                #self.label_32.setText(_translate("MainWindow", "I"))



if __name__ == "__main__":
    import sys 
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    ui = FilamentAnode(my_device = "Device_LcardE2010B")
    ui.setupUi(MainWindow)
    MainWindow.show()
    sys.exit(app.exec_())
