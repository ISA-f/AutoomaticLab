#------------------------ Qt and GUI imports --------------------------------
from PyQt5 import QtCore, QtGui, QtWidgets
from MainWindow_CloseEvent import MainWindow_withCloseEvent

#------------------------ Device imports ------------------------------------
from Device_Korad import Korad
from Lcard_EmptyDevice import LcardE2010B_EmptyDevice

#------------------------ Tab imports ---------------------------------------
from filament_and_anode_tab import FilamentAnodeTab
from Lcard_VAC_GUI import LcardVACPlot_Interface


class TabDeviceManager(object):

        def __init__(self, lcard_config = "LcardE2010B.ini", korad_config = "Korad.ini"):
                try:
                        self.myLcard = LcardE2010B_EmptyDevice(lcard_config)
                        self.myKorad = Korad(korad_config)
                        self.FilamentAnode = FilamentAnodeTab(
                                log_file = "ui_fa_test3.log",
                                lcard_device = self.myLcard,
                                korad_device = self.myKorad,
                                ControlTableConfig = "CommandTable_example.ini")
                        self.LcardVAC = LcardVACPlot_Interface(
                                Lcard_device = self.myLcard)
                except Exception as e:
                        print(e)

        def connect_lcard(self):
                # self.myLcard = LcardE2010B_EmptyDevice(lcard_config) - уже было вызвано в __init__. НЕ НАДО ВЫЗЫВАТЬ!
                try:
                        self.myLcard.ConnectToPhysicalDevice(slot=0)
                        self.myLcard.LoadConfiguration()
                except Exception as e:
                        print("try connect Lcard: ",e)

        def connect_devices(self):
                # Korad
                self.myKorad.ConnectToPhysicalDevice()

                # Lcard
                self.connect_lcard()
                return

        def setupUi(self):
                self.tabs = QtWidgets.QTabWidget()
                self.tabs.setObjectName("Tab_Device_Manager_Widget")
                self.tabs.addTab(self.FilamentAnode.setupUi(), "Filament Anode")
                self.tabs.addTab(self.LcardVAC.setupUI(), "Lcard VAC")
                return self.tabs
                

        def onCloseEvent(self):
                try:
                    self.myKorad.DisconnectFromPhysicalDevice()
                except Exception as e:
                        print("onClose Korad :", e)
                try:
                        self.myLcard.DisconnectFromPhysicalDevice()
                except Exception as e:
                        print("onClose Lcard :", e)

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
