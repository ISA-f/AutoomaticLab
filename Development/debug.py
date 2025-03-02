import LcardDataInterface as LDIF
import Device_Korad as DKorad
import Lcard_EmptyDevice
import numpy as np
import pandas as pd
import serial
from PyQt5 import QtCore, QtWidgets
import sys
import matplotlib
import matplotlib.pyplot as plt
from Updatable_QTCanvas import PyplotWidget, GraphWidget


x_data = np.random.random(10)
y_data = 2*x_data**2 + np.random.random(10)/10

data = pd.DataFrame().from_dict({"x_name" : x_data, "y_name" : y_data})
def getXYData(name_x, name_y):
    return data[name_x],data[name_y]

print("Updatable_QTCanvas.GraphWidget test0")
app = QtWidgets.QApplication(sys.argv)
MainWindow = QtWidgets.QMainWindow()
g = GraphWidget(getXYData, data.columns)
MainWindow.setCentralWidget(g.setupUI())
MainWindow.show()
app.exec_()

