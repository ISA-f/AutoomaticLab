import sys
import matplotlib
matplotlib.use('Qt5Agg')

from PyQt5 import QtCore, QtWidgets

from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure


class PyplotWidget(FigureCanvas):

    def __init__(self, parent=None, width=5, height=4, dpi=100):
        fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = fig.add_subplot(111)
        super(PyplotWidget, self).__init__(fig)

    def update_plot(self, xdata, ydata):
        self.axes.cla()  # Clear the canvas.
        self.axes.plot(xdata, ydata, 'r')
        # Trigger the canvas to update and redraw.
        self.draw()
        #print("updated plot, shape", xdata.shape, ydata.shape)

    def setAxisLabel(self, x_label, y_label):
        self.axes.set_xlabel(x_label)
        self.axes.set_ylabel(y_label)
        self.draw()



if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    p = PyplotWidget()
    p.setAxisLabel("x_hello", "y_hello")
    MainWindow.setCentralWidget(p)
    MainWindow.show()
    app.exec_()
