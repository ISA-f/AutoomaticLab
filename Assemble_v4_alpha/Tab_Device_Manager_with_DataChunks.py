#------------------------ Qt and GUI imports --------------------------------
from PyQt5 import QtCore, QtGui, QtWidgets
from MainWindow_CloseEvent import MainWindow_withCloseEvent

#------------------------ Device imports ------------------------------------
from Device_Korad import Korad
from Lcard_EmptyDevice import LcardE2010B_EmptyDevice

#------------------------ Tab imports ---------------------------------------
from Tab_Filament_and_Anode import FilamentAnodeTab
from Tab_Lcard_VAC_GUI_with_DataChunks import LcardVACPlot_Interface
from Tab_Device_Connections import TabDeviceConnections


class TabDeviceManager(object):
        def __init__(self):
                self.DeviceConnections = TabDeviceConnections()
                
                self.FilamentAnode = FilamentAnodeTab(
                                lcard_device = self.DeviceConnections.myLcard_Device,
                                korad_device = self.DeviceConnections.myKorad_Device)
                
                self.LcardVAC = LcardVACPlot_Interface(
                                Lcard_device = self.DeviceConnections.myLcard_Device)
                

        def setupUi(self):
                self.tabs = QtWidgets.QTabWidget()
                self.tabs.addTab(self.DeviceConnections.setupUi(), "Connections")
                self.tabs.addTab(self.FilamentAnode.setupUi(), "Filament Anode")
                self.tabs.addTab(self.LcardVAC.setupUI(), "Lcard VAC")
                return self.tabs
                
        def onCloseEvent(self):
                self.DeviceConnections.onCloseEvent()

def test():
    print("Tab_Device_Manager test")
    import sys 
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = MainWindow_withCloseEvent()
    MainWindow.setObjectName("MainWindow")
    MainWindow.resize(1300, 1000)
    ui = TabDeviceManager()
    centralwidget = ui.setupUi()
    MainWindow.setCentralWidget(centralwidget)
    MainWindow.CloseEventListeners.append(ui.onCloseEvent)
    MainWindow.show()
    app.exec_()


if __name__ == "__main__":
    try:
        test()
        print(">> success")
        print()
    except Exception as e:
        print(">>", e)
        a = input()
