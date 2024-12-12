from Updatable_QTCanvas import PyplotWidget

class DataFrame_to_Plot(object):
    def __init__(self, DataSource):
            self.myDataSource = DataSource
            return
    
    def setupUi(self, MainWindow):
            self.centralwidget = QtWidgets.QWidget(MainWindow)
            self.centralwidget.setObjectName("centralwidget")

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
                
    def updatePlot(self): 
            return
