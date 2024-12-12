from Updatable_QTCanvas import PyplotWidget

class DataFrame_to_Plot(object):
    def __init__(self, DataSource):
            self.myDataSource = DataSource
            return
    
    def setupUi(self, parent = None):
            # -- instantiations --
            self.myPlotWidget = QtWidgets.QWidget(parent)
            self.PlotXAxis_ComboBox = QtWidgets.QComboBox()
            self.PlotYAxis_ComboBox = QtWidgets.QComboBox()
            self.Y_x_plot = PyplotWidget()
            # -- connections to DataSource --
            self.PlotXAxis_ComboBox.addItems(self.myDataSource.myData.columns)
            self.PlotYAxis_ComboBox.addItems(self.myDataSource.myData.columns)
            # -- connections in QT --
            self.PlotXAxis_ComboBox.currentTextChanged.connect(self.updatePlot)
            self.PlotYAxis_ComboBox.currentTextChanged.connect(self.updatePlot)
            # -- layouts --
            hbox_x_axis = QtWidgets.QHBoxLayout()
            hbox_x_axis.addWidget(self.PlotXAxis_Label)
            hbox_x_axis.addWidget(self.PlotXAxis_ComboBox)
            hbox_y_axis = QtWidgets.QHBoxLayout()
            hbox_y_axis.addWidget(self.PlotYAxis_Label)
            hbox_y_axis.addWidget(self.PlotYAxis_ComboBox)
            vbox = QtWidgets.QVBoxLayout()
            vbox.addWidget(self.Y_x_plot)
            vbox.addLayout(hbox_y_axis)
            vbox.addLayout(hbox_x_axis)
            self.myPlotWidget.setLayout(vbox)
            return
                
    def updatePlot(self):
            x_label = self.PlotXAxis_ComboBox.currentText()
            y_label = self.PlotYAxis_ComboBox.currentText()
            Y_x = self.myDataSource.myData[[x_label, y_label]].dropna()
            self.Y_x_plot.update_plot(Y_x[x_label],#[max(0, Y_x.shape[0] - amount):Y_x.shape[0]],
                                      Y_x[y_label])#[max(0, Y_x.shape[0] - amount):Y_x.shape[0]])
            self.Y_x_plot.setAxisLabel(x_label, y_label)
            return
