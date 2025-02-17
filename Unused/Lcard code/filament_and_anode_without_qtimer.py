from Device_Korad import Korad
from Device_LcardE440 import LcardE440_Autoread

from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtCore import QTimer
from Device_Korad import Korad
from Device_LcardE440 import LcardE440_Autoread
from Device_LcardE2010B import LcardE2010B_Autoread

class FilamentAnode(object):

        def __init__(self, my_device = 'Device_LcardE440'):
                self.my_device = my_device # Пока что подефолту, программа не тестировалась на 2010
                self.my_confog_lcard = None
                if my_device == 'Device_LcardE440':
                        self.my_confog_lcard = "LcardE440.ini"
                        self.myLcard = LcardE440_Autoread(self.my_confog_lcard)
                elif my_device == 'Device_LcardE2010B':
                        self.my_confog_lcard = "LcardE2010B.ini"
                        self.myLcard = LcardE2010B_Autoread(self.my_confog_lcard)
                else:
                        raise NameError("invalid Lcard type")



        def setupUi(self, MainWindow):
                MainWindow.setObjectName("MainWindow")
                MainWindow.resize(825, 576)
                self.centralwidget = QtWidgets.QWidget(MainWindow)
                self.centralwidget.setObjectName("centralwidget")
                self.label_light_gray_filament = QtWidgets.QLabel(self.centralwidget)
                self.label_light_gray_filament.setGeometry(QtCore.QRect(150, 100, 241, 321))
                self.label_light_gray_filament.setStyleSheet("background-color: rgb(200, 200, 200);")
                self.label_light_gray_filament.setText("")
                self.label_light_gray_filament.setObjectName("label_light_gray_filament")
                self.label_filament = QtWidgets.QLabel(self.centralwidget)
                self.label_filament.setGeometry(QtCore.QRect(234, 100, 91, 51))
                self.label_filament.setStyleSheet("font: 500 18pt \"Tahoma\";\n"
                                                  "color:rgb(255, 255, 255)")
                self.label_filament.setObjectName("label_filament")
                self.label_dark_gray_filament = QtWidgets.QLabel(self.centralwidget)
                self.label_dark_gray_filament.setGeometry(QtCore.QRect(150, 100, 241, 51))
                self.label_dark_gray_filament.setStyleSheet("background-color: rgb(134, 134, 134);")
                self.label_dark_gray_filament.setText("")
                self.label_dark_gray_filament.setObjectName("label_dark_gray_filament")
                self.label_I_filament = QtWidgets.QLabel(self.centralwidget)
                self.label_I_filament.setGeometry(QtCore.QRect(170, 170, 31, 31))
                self.label_I_filament.setStyleSheet("font: 500 18pt \"Tahoma\";\n"
                                                    "color: rgb(231, 76, 76)")
                self.label_I_filament.setObjectName("label_I_filament")
                self.line_filament = QtWidgets.QFrame(self.centralwidget)
                self.line_filament.setGeometry(QtCore.QRect(150, 142, 241, 20))
                self.line_filament.setFrameShape(QtWidgets.QFrame.HLine)
                self.line_filament.setFrameShadow(QtWidgets.QFrame.Sunken)
                self.line_filament.setObjectName("line_filament")
                self.label_A_filament = QtWidgets.QLabel(self.centralwidget)
                self.label_A_filament.setGeometry(QtCore.QRect(330, 170, 21, 31))
                self.label_A_filament.setStyleSheet("font: 500 18pt \"Tahoma\";\n"
                                                    "color: rgb(231, 76, 76)")
                self.label_A_filament.setObjectName("label_A_filament")
                self.lcdNumber_I_filament = QtWidgets.QLCDNumber(self.centralwidget)
                self.lcdNumber_I_filament.setGeometry(QtCore.QRect(220, 170, 101, 31))
                self.lcdNumber_I_filament.setStyleSheet("background-color: rgb(59, 59, 59);\n"
                                                        "color: rgb(255, 0, 0);")
                self.lcdNumber_I_filament.setObjectName("lcdNumber_I_filament")
                self.label_B_filament = QtWidgets.QLabel(self.centralwidget)
                self.label_B_filament.setGeometry(QtCore.QRect(330, 220, 21, 31))
                self.label_B_filament.setStyleSheet("font: 500 18pt \"Tahoma\";\n"
                                                    "color: rgb(231, 76, 76)")
                self.label_B_filament.setObjectName("label_B_filament")
                self.label_U_filament = QtWidgets.QLabel(self.centralwidget)
                self.label_U_filament.setGeometry(QtCore.QRect(165, 220, 31, 31))
                self.label_U_filament.setStyleSheet("font: 500 18pt \"Tahoma\";\n"
                                                    "color: rgb(231, 76, 76)")
                self.label_U_filament.setObjectName("label_U_filament")
                self.label_mO_filament = QtWidgets.QLabel(self.centralwidget)
                self.label_mO_filament.setGeometry(QtCore.QRect(327, 270, 81, 31))
                self.label_mO_filament.setStyleSheet("font: 500 18pt \"Tahoma\";\n"
                                                     "color: rgb(231, 76, 76)")
                self.label_mO_filament.setObjectName("label_mO_filament")
                self.label_R_filament = QtWidgets.QLabel(self.centralwidget)
                self.label_R_filament.setGeometry(QtCore.QRect(168, 270, 31, 31))
                self.label_R_filament.setStyleSheet("font: 500 18pt \"Tahoma\";\n"
                                                    "color: rgb(231, 76, 76)")
                self.label_R_filament.setObjectName("label_R_filament")
                self.label_W_filament = QtWidgets.QLabel(self.centralwidget)
                self.label_W_filament.setGeometry(QtCore.QRect(330, 320, 41, 31))
                self.label_W_filament.setStyleSheet("font: 500 18pt \"Tahoma\";\n"
                                                    "color: rgb(231, 76, 76)")
                self.label_W_filament.setObjectName("label_W_filament")
                self.label_P_filament = QtWidgets.QLabel(self.centralwidget)
                self.label_P_filament.setGeometry(QtCore.QRect(169, 320, 31, 31))
                self.label_P_filament.setStyleSheet("font: 500 18pt \"Tahoma\";\n"
                                                    "color: rgb(231, 76, 76)")
                self.label_P_filament.setObjectName("label_P_filament")
                self.label_K_filament = QtWidgets.QLabel(self.centralwidget)
                self.label_K_filament.setGeometry(QtCore.QRect(330, 370, 21, 31))
                self.label_K_filament.setStyleSheet("font: 500 18pt \"Tahoma\";\n"
                                                    "color: rgb(231, 76, 76)")
                self.label_K_filament.setObjectName("label_K_filament")
                self.label_T_filament = QtWidgets.QLabel(self.centralwidget)
                self.label_T_filament.setGeometry(QtCore.QRect(168, 370, 31, 31))
                self.label_T_filament.setStyleSheet("font: 500 18pt \"Tahoma\";\n"
                                                    "color: rgb(231, 76, 76)")
                self.label_T_filament.setObjectName("label_T_filament")
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
                self.pushButton_start = QtWidgets.QPushButton(self.centralwidget)
                self.pushButton_start.setGeometry(QtCore.QRect(150, 440, 151, 51))
                self.pushButton_start.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.pushButton_start.setObjectName("pushButton_start")
                self.pushButton_stop = QtWidgets.QPushButton(self.centralwidget)
                self.pushButton_stop.setGeometry(QtCore.QRect(320, 440, 151, 51))
                self.pushButton_stop.setStyleSheet("font: 75 18pt \"Tahoma\";")
                self.pushButton_stop.setObjectName("pushButton_stop")
                self.lcdNumber_U_filament = QtWidgets.QLCDNumber(self.centralwidget)
                self.lcdNumber_U_filament.setGeometry(QtCore.QRect(220, 220, 101, 31))
                self.lcdNumber_U_filament.setStyleSheet("background-color: rgb(59, 59, 59);\n"
                                                        "color: rgb(255, 0, 0);")
                self.lcdNumber_U_filament.setObjectName("lcdNumber_U_filament")
                self.lcdNumber_R_filament = QtWidgets.QLCDNumber(self.centralwidget)
                self.lcdNumber_R_filament.setGeometry(QtCore.QRect(220, 270, 101, 31))
                self.lcdNumber_R_filament.setStyleSheet("background-color: rgb(59, 59, 59);\n"
                                                        "color: rgb(255, 0, 0);")
                self.lcdNumber_R_filament.setObjectName("lcdNumber_R_filament")
                self.lcdNumber_P_filament = QtWidgets.QLCDNumber(self.centralwidget)
                self.lcdNumber_P_filament.setGeometry(QtCore.QRect(220, 320, 101, 31))
                self.lcdNumber_P_filament.setStyleSheet("background-color: rgb(59, 59, 59);\n"
                                                        "color: rgb(255, 0, 0);")
                self.lcdNumber_P_filament.setObjectName("lcdNumber_P_filament")
                self.lcdNumber_T_filament = QtWidgets.QLCDNumber(self.centralwidget)
                self.lcdNumber_T_filament.setGeometry(QtCore.QRect(220, 370, 101, 31))
                self.lcdNumber_T_filament.setStyleSheet("background-color: rgb(59, 59, 59);\n"
                                                        "color: rgb(255, 0, 0);")
                self.lcdNumber_T_filament.setObjectName("lcdNumber_T_filament")
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
                self.label_light_gray_anode.raise_()
                self.label_light_gray_filament.raise_()
                self.label_dark_gray_filament.raise_()
                self.label_filament.raise_()
                self.label_I_filament.raise_()
                self.line_filament.raise_()
                self.label_A_filament.raise_()
                self.lcdNumber_I_filament.raise_()
                self.label_B_filament.raise_()
                self.label_U_filament.raise_()
                self.label_mO_filament.raise_()
                self.label_R_filament.raise_()
                self.label_W_filament.raise_()
                self.label_P_filament.raise_()
                self.label_K_filament.raise_()
                self.label_T_filament.raise_()
                self.label_dark_gray_anode.raise_()
                self.label_mA_for_Ia_anode.raise_()
                self.label_mA_for_sigmaI.raise_()
                self.label_mA_for_Imin_anode.raise_()
                self.label_anode.raise_()
                self.label_Ia_anode.raise_()
                self.line_anode.raise_()
                self.label_sigmaI_anode.raise_()
                self.label_B_for_Ua_anode.raise_()
                self.label_Imax_anode.raise_()
                self.label_mA_for_Imax_anode.raise_()
                self.label_Imin_anode.raise_()
                self.label_Ua_anode.raise_()
                self.label_28.raise_()
                self.label_a_for_U_anode.raise_()
                self.label_30.raise_()
                self.label_31.raise_()
                self.label_32.raise_()
                self.pushButton_start.raise_()
                self.pushButton_stop.raise_()
                self.lcdNumber_U_filament.raise_()
                self.lcdNumber_R_filament.raise_()
                self.lcdNumber_P_filament.raise_()
                self.lcdNumber_T_filament.raise_()
                self.lcdnumber_sigmaI_anode.raise_()
                self.lcdnumber_Ua_anode.raise_()
                self.lcdnumber_Imin_anode.raise_()
                self.lcdnumber_Imax_anode.raise_()
                self.lcdnumber_Ia_anode.raise_()
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
                self.label_mA_for_Ia_anode.setText(_translate("MainWindow", "mA"))
                self.label_mA_for_sigmaI.setText(_translate("MainWindow", "mA"))
                self.label_mA_for_Imin_anode.setText(_translate("MainWindow", "mA"))
                self.label_anode.setText(_translate("MainWindow", "Анод"))
                self.label_Ia_anode.setText(_translate("MainWindow", "I"))
                self.label_sigmaI_anode.setText(_translate("MainWindow", "o"))
                self.label_B_for_Ua_anode.setText(_translate("MainWindow", "B"))
                self.label_Imax_anode.setText(_translate("MainWindow", "I"))
                self.label_mA_for_Imax_anode.setText(_translate("MainWindow", "mA"))
                self.label_Imin_anode.setText(_translate("MainWindow", "I"))
                self.label_Ua_anode.setText(_translate("MainWindow", "U"))
                self.label_28.setText(_translate("MainWindow", "a"))
                self.label_a_for_U_anode.setText(_translate("MainWindow", "a"))
                self.label_30.setText(_translate("MainWindow", "min"))
                self.label_31.setText(_translate("MainWindow", "max"))
                self.label_32.setText(_translate("MainWindow", "I"))
                self.pushButton_start.setText(_translate("MainWindow", "Start"))
                self.pushButton_stop.setText(_translate("MainWindow", "Stop"))

                self.pushButton_start.clicked.connect(self.start_filament_anode)

        def connect_korad(self):
                self.myKorad = Korad('Korad.ini')
                self.myKorad.ConnectToPhysicalDevice()

        def connect_lcard(self):
                #self.myLcard = LcardE440_Autoread("LcardE440.ini") - уже было вызвано в __init__. НЕ НАДО ВЫЗЫВАТЬ!
                self.myLcard.ConnectToPhysicalDevice(slot=0)
                self.myLcard.LoadConfiguration()#hotfix=True)
                self.myLcard.StartMeasurements("filament_Lcard.log")

        def connect_devices(self):
                # Korad
                #self.connect_korad()

                # Lcard
                self.connect_lcard()
                pass

        def start_filament_anode(self):
                #print("Hello Sanya")
                self.connect_devices()
                time.sleep(10)
                self.myLcard.FinishMeasurements()
                """
                try:
                    self.timer.timeout.connect(self.updata_filament_anode)
                    self.timer.start(20)
                except Exception as inst:
                        print(inst)
                        raise inst
                """

        def updata_filament_anode(self):
                print("Start update")
                """
                # for filament
                time_sistem, voltage, current = self.myKorad.TakeMeasurements()

                self.lcdNumber_I_filament.display("{:05.3f}".format(current))
                self.lcdNumber_U_filament.display("{:05.3f}".format(voltage))
                try:
                        float(voltage)/float(current)

                except:
                        self.lcdNumber_R_filament.display('0/0')
                self.lcdNumber_P_filament.display("{:05.3f}".format(voltage*current))
                """
                # for anode
                Data = myLcard.ReadFlashDataAveraged()
                print(Data)
                lcard_time = Data[0]
                ch1_mean = Data[1]
                ch1_var = Data[2]
                ch1_min = Data[3]
                ch1_max = Data[4]
                ch2_mean = Data[5]
                ch2_var = Data[6]
                ch2_min = Data[7]
                ch2_max = Data[8]

                config = configparser.ConfigParser()
                config.read(self.my_confog_lcard)
                k1 = config['Constants']['k1']
                k2 = config['Constants']['k2']
                c1 = config['Constants']['c1']
                c2 = config['Constants']['c2']

                # Тут формулки, их желательно проверить на корректность

                self.lcdnumber_Ua_anode.display("{:05.3f}".format(k2*ch1_mean)) # Ua = k1 <ch1>
                self.lcdnumber_Ia_anode.display("{:05.3f}".format(c1*ch1_mean - c2*ch2_mean)) # Ia = c1 <ch1> - c2 <ch2>
                self.lcdnumber_Imin_anode.display("{:05.3f}".format(c1*ch1_min - c2*ch2_max)) # Imin = c1 ch1_min - c2 ch2_max
                self.lcdnumber_Imin_anode.display("{:05.3f}".format(c1 * ch1_max - c2*ch2_min))  # Imin = c1 ch1_max - c2 ch2_min
                self.lcdnumber_sigmaI_anode.display("{:05.3f}".format(c1*np.sqrt(ch1_var) - c2*np.sqrt(ch2_var))) # sigma = c1 sigma_1 - c2 sigma_2


if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    ui = FilamentAnode(my_device = "Device_LcardE2010B")
    ui.setupUi(MainWindow)
    MainWindow.show()
    sys.exit(app.exec_())
