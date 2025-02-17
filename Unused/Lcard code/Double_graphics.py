from PyQt5 import (QtCore,
                   QtGui,
                   QtWidgets,
                   QtTest)
from PyQt5.QtCore import QTimer

from Device_LcardE440 import LcardE440_Autoread
from Device_LcardE2010B import LcardE2010B_Autoread

from PyQt5.QtWidgets import QVBoxLayout

from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar
from matplotlib.figure import Figure
import matplotlib.animation as animation
import matplotlib
import matplotlib.pyplot as plt

import time
import numpy as np
from tabulate import tabulate
import serial

from Device_Korad import Korad

class Ui_MainWindow(QtWidgets.QMainWindow):

    def __init__(self, my_device='Device_LcardE440'):
        self.my_device = my_device  # Пока что подефолту, программа не тестировалась на 2010
        self.my_confog_lcard = None
        if my_device == 'Device_LcardE440':
            self.my_confog_lcard = "LcardE440.ini"
            self.myLcard = LcardE440_Autoread(self.my_confog_lcard)
        if my_device == 'Device_LcardE2010B':
            self.my_confog_lcard = "LcardE2010B.ini"
            self.myLcard = LcardE440_Autoread(self.my_confog_lcard)


    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(1243, 702)
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.graphicsView_korad = QtWidgets.QGraphicsView(self.centralwidget)
        self.graphicsView_korad.setGeometry(QtCore.QRect(25, 60, 561, 521))
        self.graphicsView_korad.setObjectName("graphicsView_korad")
        self.graphicsView_lcard = QtWidgets.QGraphicsView(self.centralwidget)
        self.graphicsView_lcard.setGeometry(QtCore.QRect(645, 60, 561, 521))
        self.graphicsView_lcard.setObjectName("graphicsView_lcard")
        self.label_korad = QtWidgets.QLabel(self.centralwidget)
        self.label_korad.setGeometry(QtCore.QRect(270, 10, 91, 41))
        self.label_korad.setStyleSheet("font: 500 18pt \"Tahoma\";\n"
"color: rgb(0, 0, 255);")
        self.label_korad.setObjectName("label_korad")
        self.label_lcard = QtWidgets.QLabel(self.centralwidget)
        self.label_lcard.setGeometry(QtCore.QRect(900, 10, 81, 41))
        self.label_lcard.setStyleSheet("font: 500 18pt \"Tahoma\";\n"
"color: rgb(0, 0, 255);")
        self.label_lcard.setObjectName("label_lcard")
        self.pushButton_start = QtWidgets.QPushButton(self.centralwidget)
        self.pushButton_start.setGeometry(QtCore.QRect(23, 610, 151, 51))
        self.pushButton_start.setStyleSheet("font: 75 18pt \"Tahoma\";")
        self.pushButton_start.setObjectName("pushButton_start")
        self.pushButton_stop = QtWidgets.QPushButton(self.centralwidget)
        self.pushButton_stop.setGeometry(QtCore.QRect(193, 610, 151, 51))
        self.pushButton_stop.setStyleSheet("font: 75 18pt \"Tahoma\";")
        self.pushButton_stop.setObjectName("pushButton_stop")
        MainWindow.setCentralWidget(self.centralwidget)
        self.statusbar = QtWidgets.QStatusBar(MainWindow)
        self.statusbar.setObjectName("statusbar")
        MainWindow.setStatusBar(self.statusbar)

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "MainWindow"))
        self.label_korad.setText(_translate("MainWindow", "Korad"))
        self.label_lcard.setText(_translate("MainWindow", "Lcard"))
        self.pushButton_start.setText(_translate("MainWindow", "Start"))
        self.pushButton_stop.setText(_translate("MainWindow", "Stop"))

        self.pushButton_start.clicked.connect(self.graphics)
        self.pushButton_stop.clicked.connect(self.stop_measuring)

    def stop_measuring(self):
        self.timer.stop()
        self.myLcard.StopMeasurements()
        self.ser.close()

    def lcard_data(self, old_data:np.array):
        print(old_data)
        print(self.myLcard.ReadFlashDataAveraged())
        Data = np.column_stack([self.myLcard.ReadFlashDataAveraged(), old_data])
        return Data


    def start_lcard(self):
        # self.myLcard = LcardE440_Autoread("LcardE440.ini") - уже было вызвано в __init__. НЕ НАДО ВЫЗЫВАТЬ!
        self.myLcard.ConnectToPhysicalDevice(slot=0)
        self.myLcard.LoadConfiguration()
        self.myLcard.StartMeasurements("auto_channel1.log")


    def start_korad(self):
        self.myKorad = Korad('Korad.ini')
        self.myKorad.ConnectToPhysicalDevice()


    def graphics(self):
        # Lcard
        self.start_lcard()
        vbox2 = QVBoxLayout(self.graphicsView_lcard)
        self.figure2 = matplotlib.figure.Figure()
        self.canvas2 = FigureCanvas(self.figure2)
        self.toolbar2 = NavigationToolbar(self.canvas2, self)
        vbox2.addWidget(self.canvas2)
        vbox2.addWidget(self.toolbar2)

        self.x2 = []
        self.y2 = []

        self.axes2 = self.figure2.add_subplot(111)
        self.axes2.scatter([], [], color='b')

        # Korad
        self.start_korad()
        vbox = QVBoxLayout(self.graphicsView_korad)
        self.figure = matplotlib.figure.Figure()
        self.canvas = FigureCanvas(self.figure)
        self.toolbar = NavigationToolbar(self.canvas, self)
        vbox.addWidget(self.canvas)
        vbox.addWidget(self.toolbar)

        self.x = []
        self.y = []

        self.axes = self.figure.add_subplot(111)
        self.axes.scatter([], [], color='b')

        # Animation graphics
        self.ani_korad = animation.FuncAnimation(self.figure,
                                           self.updata_korad,
                                           frames=100,
                                           interval=100,
                                           repeat=True)

        self.ani_lcard = animation.FuncAnimation(self.figure,
                                                 self.updata_lcard,
                                                 frames=100,
                                                 interval=100,
                                                 repeat=True)

        self.figure.canvas.draw_idle()
        self.figure2.canvas.draw_idle()

    def updata_korad(self, i):
        # Korad
        time_sistem, voltage, current = self.myKorad.TakeMeasurements()
        self.axes.scatter(voltage, current, s=1, c='b')

    def updata_lcard(self, i):
        # Lcard
        Data2 = self.myLcard.ReadFlashDataAveraged()
        self.axes2.scatter(Data2[0], Data2[1], s=1, c='g')
        self.axes2.scatter(Data2[0], Data2[1] - np.sqrt(Data2[2]), s=1, c='orange')
        self.axes2.scatter(Data2[0], Data2[1] + np.sqrt(Data2[2]), s=1, c='orange')
        self.axes2.scatter(Data2[0], Data2[3], s=1, c='r')
        self.axes2.scatter(Data2[0], Data2[4], s=1, c='r')


if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    ui = Ui_MainWindow()
    ui.setupUi(MainWindow)
    MainWindow.show()
    sys.exit(app.exec_())
