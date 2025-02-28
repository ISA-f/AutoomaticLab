import LcardDataInterface as LDIF
import Lcard_EmptyDevice
import numpy as np
import pandas as pd

data = np.random.random((4,10))
DLcard = Lcard_EmptyDevice.LcardE2010B_EmptyDevice("LcardE2010B.ini")
LcardIF = LDIF.LcardDataInterface(DLcard)

N_channels, N_measurements = data.shape[0], data.shape[1]
LcardIF.data = pd.DataFrame(data.T, columns = LDIF.RawChannelNames[:N_channels])
LcardIF.data[LDIF.LCARD_NAMES.INDEX] = np.arange(N_measurements)
print(LcardIF.data.drop(LDIF.LCARD_NAMES.INDEX, axis = 1))

LDIF.calculateAverage(LcardIF)
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
