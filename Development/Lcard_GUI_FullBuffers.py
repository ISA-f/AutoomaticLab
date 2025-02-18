import numpy as np
# ------------------- Lcard imports -------------------------
from Lcard_IF_FullBuffers import Lcard_Interface_FullBuffers
# -------------------- GUI imports -------------------------
from DataFrame_to_Plot import DataFrame_to_Plot
from PyQt5 import QtWidgets



class Lcard_GUI_FullBuffers(object):
    def __init__(self, Lcard_Interface_FullBuffers, parent = None):
        self.myLIF_FullBuffers = Lcard_Interface_FullBuffers
        
    #NEEDED np.array -> pd.DataFrame
    @property
    def myData(self):
        return self.myLIF_FullBuffers.myData

    def setupUI(self):
        self.
        self.myPyplot = PyplotWidget(parent = None)
        self.myPyplot.setAxisLabel(xlabel = "time",
                                   ylabel = "LcardChannel")
        self.PlotXAxis_Label = QtWidgets.QLabel("X axis", self.centralwidget)
        self.PlotYAxis_Label = QtWidgets.QLabel("Y axis", self.centralwidget)
        self.PlotXAxis_ComboBox = QtWidgets.QComboBox(self.centralwidget)
        self.PlotYAxis_ComboBox = QtWidgets.QComboBox(self.centralwidget)
        self.PlotYAxis_ComboBox.addItems(self.myData.columns)

    def updateData(self):
        self.myData = self.myLIF_FullBuffers.myData

    def update_plot(self, channel):
        y = self.myData[:, channel]
        x = np.arange(len(y))
        self.myPyplot.update_plot(x,y)
"""
        
    
