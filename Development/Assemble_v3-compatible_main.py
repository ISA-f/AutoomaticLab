#------------------------ Qt and GUI imports --------------------------------
from PyQt5 import QtCore, QtGui, QtWidgets
from MainWindow_CloseEvent import MainWindow_withCloseEvent
import sys

#------------------------ Korad imports ------------------------------------
from Device_Korad import Korad

#------------------------ Lcard imports ------------------------------------
import Lcard_EmptyDevice
import LcardDataInterface as LDIF

#------------------------ main GUI window -------------------------
import filament_and_anode_tab



if __name__ == "__main__":
    myLcard = Lcard_EmptyDevice.LcardE2010B_EmptyDevice("LcardE2010B.ini")
    myKorad = Korad('Korad.ini')

    app = QtWidgets.QApplication(sys.argv)
    MainWindow = MainWindow_withCloseEvent()
    ui = filament_and_anode_tab.FilamentAnodeTab(log_file = "ui_fa_test3.log", 
                          lcard_device = myLcard, korad_device = myKorad)
    centralwidget = ui.setupUi()
    MainWindow.setCentralWidget(centralwidget)
    MainWindow.CloseEventListeners.append(ui.onCloseEvent)
    MainWindow.show()
    sys.exit(app.exec_())
