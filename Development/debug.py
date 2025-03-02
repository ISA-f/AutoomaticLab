import LcardDataInterface as LDIF
import Device_Korad as DKorad
import Lcard_EmptyDevice
import numpy as np
import pandas as pd
import serial

ldif = LDIF.LcardDataInterface("debug")
ldif.data = np.random.random((4, 8000))
ldif.syncd = 1000
LDIF.cropToRequestedBuffer(ldif, 100)
LDIF.calculateAverage(ldif)
print(ldif.data)

ldif = LDIF.LcardDataInterface(None)
LDIF.calculateAverage(ldif)
print(ldif.data)
"""
def test():
    print("Lcard VAC GUI test")
    import sys
    import Lcard_EmptyDevice
    import Tab_Lcard_VAC_GUI
    from PyQt5 import QtCore, QtGui, QtWidgets
    
    Lcard_Device = Lcard_EmptyDevice.LcardE2010B_EmptyDevice("LcardE2010B.ini")
    ui = Tab_Lcard_VAC_GUI.LcardVACPlot_Interface(Lcard_Device)
    
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    LVACwidget = ui.setupUI()
    MainWindow.setCentralWidget(LVACwidget)
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
"""
