import sys
import matplotlib
matplotlib.use('Qt5Agg')

from PyQt5 import QtCore, QtWidgets

from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure

import numpy as np

class PyplotWidget(FigureCanvas):

    def __init__(self, parent=None, width=5, height=4, dpi=100):
        self.fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = self.fig.add_subplot(111)
        super(PyplotWidget, self).__init__(self.fig)

    def update_plot(self, xdata, ydata, s = 15):
        self.axes.cla()  # Clear the canvas.
        self.axes.plot(xdata, ydata, color = 'r')
        self.axes.scatter(xdata, ydata, color = 'black', s = s)
        self.draw()

    def setAxisLabel(self, x_label, y_label):
        self.axes.set_xlabel(x_label)
        self.axes.set_ylabel(y_label)
        self.draw()


class GraphWidget(object):
    def __init__(self, getXYData, DataColumns):
        self.last_plot = None
        self.last_scatter = None
        self.getXYData = getXYData
        self.DataColumns = DataColumns
        self.last_x_data = None
        self.last_y_data = None
        self.x_low_bound = 0
        self.x_high_bound = 1
        self.y_low_bound = 0
        self.y_high_bound = 1

    def setupUI(self):
        self.centralwidget = QtWidgets.QWidget()
        self._translate = QtCore.QCoreApplication.translate
        self.Pyplot = PyplotWidget()

        # --- Clear ---
        self.QpushButton_Clear = QtWidgets.QPushButton(self.centralwidget)
        self.QpushButton_Clear.setStyleSheet("font: 75 18pt \"Tahoma\";")
        self.QpushButton_Clear.setObjectName("Clear")
        self.QpushButton_Clear.setText(self._translate("MainWindow", "Clear"))
        self.QpushButton_Clear.setEnabled(True)
        self.QpushButton_Clear.clicked.connect(self.pushClearButton)

        # --- Draw ---
        self.QpushButton_Draw = QtWidgets.QPushButton(self.centralwidget)
        self.QpushButton_Draw.setStyleSheet("font: 75 18pt \"Tahoma\";")
        self.QpushButton_Draw.setObjectName("Clear")
        self.QpushButton_Draw.setText(self._translate("MainWindow", "Draw"))
        self.QpushButton_Draw.setEnabled(True)
        self.QpushButton_Draw.clicked.connect(self.pushDrawButton)

        # --- Plot ComboBoxes ---
        self.PlotXAxis_ComboBox = QtWidgets.QComboBox(self.centralwidget)
        self.PlotXAxis_ComboBox.addItems(self.DataColumns)
        self.PlotXAxis_ComboBox.currentTextChanged.connect(self.drawLine)
        self.PlotYAxis_ComboBox = QtWidgets.QComboBox(self.centralwidget)
        self.PlotYAxis_ComboBox.addItems(self.DataColumns)
        self.PlotYAxis_ComboBox.currentTextChanged.connect(self.drawLine)

        # --- xlimit ylimit LineEdits
        self.QLineEdit_x_low_bound = QtWidgets.QLineEdit()
        self.QLineEdit_x_high_bound = QtWidgets.QLineEdit()
        self.QLineEdit_y_low_bound = QtWidgets.QLineEdit()
        self.QLineEdit_y_high_bound = QtWidgets.QLineEdit()
        self.QLineEdit_x_low_bound.editingFinished.connect(self.onGraphLimitChange)
        self.QLineEdit_x_high_bound.editingFinished.connect(self.onGraphLimitChange)
        self.QLineEdit_y_low_bound.editingFinished.connect(self.onGraphLimitChange)
        self.QLineEdit_y_high_bound.editingFinished.connect(self.onGraphLimitChange)
        self.QLayout_XYBounds = QtWidgets.QHBoxLayout()
        self.QLayout_XYBounds.addWidget(self.QLineEdit_x_low_bound)
        self.QLayout_XYBounds.addWidget(self.QLineEdit_y_low_bound)
        self.QLayout_XYBounds.addWidget(self.QLineEdit_x_high_bound)
        self.QLayout_XYBounds.addWidget(self.QLineEdit_y_high_bound)
        
        # --- Layouts ---
        self.QLayout_General = QtWidgets.QVBoxLayout()
        self.QLayout_General.addWidget(self.Pyplot)
        self.QLayout_General.addWidget(self.QpushButton_Clear)
        self.QLayout_General.addWidget(self.QpushButton_Draw)
        self.QLayout_General.addWidget(self.PlotXAxis_ComboBox)
        self.QLayout_General.addWidget(self.PlotYAxis_ComboBox)
        self.QLayout_General.addLayout(self.QLayout_XYBounds)
        self.centralwidget.setLayout(self.QLayout_General)
        
        return self.centralwidget

    def drawLine(self):
        try:
            self.removeLastLine()
            # --- data filtering ---
            x_label, y_label = self.PlotXAxis_ComboBox.currentText(), self.PlotYAxis_ComboBox.currentText()
            self.last_x_data, self.last_y_data = self.getXYData(x_label, y_label)
            mask = self.last_x_data.notna()*self.last_y_data.notna()
            # --- drawing ---
            self.last_plot, = self.Pyplot.axes.plot(self.last_x_data[mask], self.last_y_data[mask], alpha = 0.2)
            self.last_scatter = self.Pyplot.axes.scatter(self.last_x_data[mask], self.last_y_data[mask])
            self.setAxisLabels(x_label, y_label)
            self.Pyplot.draw()
            # --- QtWidgets update ---
            self.QpushButton_Draw.setEnabled(True)
        except Exception as e:
            print(e)

    def removeLastLine(self):
        if not(self.last_plot is None):
            self.last_plot.remove()
            self.last_plot = None
        if not(self.last_scatter is None):
            self.last_scatter.remove()
            self.last_scatter = None

    def maskXYBounds(self):
        return (self.last_x_data >= self.x_low_bound)*(self.last_x_data <= self.x_high_bound)*(self.last_y_data >= self.y_low_bound)*(self.last_x_data <= self.y_high_bound)

    def pushDrawButton(self):
        mask = self.last_x_data.notna()*self.last_y_data.notna()*self.maskXYBounds()
        self.last_plot = None
        self.last_scatter = None
        self.last_x_data = None
        self.last_y_data = None
        self.QpushButton_Draw.setEnabled(False)
        
    def pushClearButton(self):
        self.Pyplot.axes.cla() # Clear the canvas.
        self.Pyplot.draw()

    def onGraphLimitChange(self):
        try:
            self.x_low_bound = float(self.QLineEdit_x_low_bound.text())
            self.Pyplot.axes.set_xlim(left = self.x_low_bound)
        except Exception as e:
            print(e)
        try:
            self.x_high_bound = float(self.QLineEdit_x_high_bound.text())
            self.Pyplot.axes.set_xlim(right = self.x_high_bound)
        except Exception as e:
            print(e)
        try:
            self.y_low_bound = float(self.QLineEdit_y_low_bound.text())
            self.Pyplot.axes.set_ylim(bottom = self.y_low_bound)
        except Exception as e:
            print(e)
        try:
            self.y_high_bound = float(self.QLineEdit_y_high_bound.text())
            self.Pyplot.axes.set_ylim(top =  self.y_high_bound)
        except Exception as e:
            print(e)
        self.Pyplot.draw()

    def setAxisLabels(self, x_label, y_label):
        self.Pyplot.axes.set_xlabel(x_label)
        self.Pyplot.axes.set_ylabel(y_label)
        self.draw()

        



def test0():
    print("Updatable_QTCanvas.PyplotWidget test0")
    import numpy as np
    data = np.random.random((2,10))
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    g = GraphWidget(1)
    MainWindow.setCentralWidget(g.setupUI())
    print(type(g.Pyplot.axes))
    MainWindow.show()
    app.exec_()

def test():
    print("Updatable_QTCanvas.PyplotWidget test")
    import numpy as np
    data = np.random.random((2,10))
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    p = PyplotWidget()
    p.setAxisLabel("x_hello", "y_hello")
    p.update_plot(data[0], data[1])
    MainWindow.setCentralWidget(p)
    MainWindow.show()
    app.exec_()

if __name__ == "__main__":
    try:
        test0()
        print(">> success")
    except Exception as e:
        print(">>",e)
