#------------------------ Qt and GUI imports --------------------------------
from PyQt5 import QtCore, QtGui, QtWidgets
from MainWindow_CloseEvent import MainWindow_withCloseEvent

#------------------------ Device imports ------------------------------------
from Device_Korad import Korad
from Lcard_EmptyDevice import LcardE2010B_EmptyDevice

#------------------------ Tab imports ---------------------------------------
from filament_and_anode_tab import FilamentAnodeTab


class TabDeviceManager(object):

        def __init__(self, my_device = 'Device_LcardE2010B'):
                self.my_device = my_device
                if my_device == 'Device_LcardE2010B':
                        self.my_config_lcard = "LcardE2010B.ini"
                        self.myLcard = LcardE2010B_EmptyDevice(self.my_config_lcard)
                        self.myKorad = Korad('Korad.ini')
                else:
                        raise NameError("invalid Lcard type")

        def connect_lcard(self):
                # self.myLcard = LcardE440_Autoread("LcardE440.ini") - уже было вызвано в __init__. НЕ НАДО ВЫЗЫВАТЬ!
                print("try connect lcard")
                try:
                        self.myLcard.ConnectToPhysicalDevice(slot=0)
                        self.myLcard.LoadConfiguration()
                except Exception as e:
                        print(e)

        def connect_devices(self):
                # Korad
                self.myKorad.ConnectToPhysicalDevice()

                # Lcard
                self.connect_lcard()
                return

        def setupUi(self, MainWindow):
                MainWindow.setObjectName("MainWindow")
                MainWindow.resize(1300, 900)
                
                self.centralwidget = QtWidgets.QWidget(MainWindow)
                self.centralwidget.setObjectName("centralwidget")

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

                #Lcard
                #self.myLcardwidget = QtWidgets.QWidget(self.centralwidget)
                #self.myLcardwidget.setObjectName("Lcard widget")
                #self.myLcardwidget.setGeometry(QtCore.QRect(400, 500, 300, 300))
                #self.myLcardInterface = Lcard_Interface.LcardE2010B_Interface(self.myLcardwidget)
                #self.myLcardInterface.SetupUI()
                
                MainWindow.setCentralWidget(self.centralwidget)
                self.statusbar = QtWidgets.QStatusBar(MainWindow)
                self.statusbar.setObjectName("statusbar")
                MainWindow.setStatusBar(self.statusbar)
                
                _translate = QtCore.QCoreApplication.translate
                MainWindow.setWindowTitle(_translate("MainWindow", "MainWindow"))

                #start-stop pushButtons
                self.pushButton_start.setText(_translate("MainWindow", "Start"))
                self.pushButton_stop.setText(_translate("MainWindow", "Stop"))
                self.pushButton_start.clicked.connect(self.start_filament_anode)
                self.pushButton_stop.clicked.connect(self.FinishMeasurements)
                QtCore.QMetaObject.connectSlotsByName(MainWindow)

        def onCloseEvent(self):
                try:
                    self.myKorad.DisconnectFromPhysicalDevice()
                except Exception as e:
                        print("onClose Korad", e)
                try:
                        self.myLcard.FinishMeasurements()
                        self.myLcard.DisconnectFromPhysicalDevice()
                except Exception as e:
                        print("onClose Lcard, ", e)



if __name__ == "__main__":
    import sys 
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = MainWindow_withCloseEvent()
    ui = TabDeviceManager()
    ui.setupUi(MainWindow)
    MainWindow.CloseEventListeners.append(ui.onCloseEvent)
    MainWindow.show()
    sys.exit(app.exec_())
