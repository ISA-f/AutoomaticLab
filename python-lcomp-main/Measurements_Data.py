import pandas as pd
from PyQt5.QtCore import pyqtSignal, QObject



class Measurements_Data(QObject):
    changed = pyqtSignal()
    def __init__(self, *args, **kwargs):
        super().__init__()
        self.__Data = pd.DataFrame(*args, **kwargs)
        self.myWidget = Measurements_Data_Widget()

    @property
    def Data(self):
        return self.__Data

    @Data.setter
    def Data(self, value):
        self.__Data = value
        self.changed.emit()
        return None



def hello2():
    print("changed2")

if __name__ == "__main__":
    import sys
    from PyQt5.QtWidgets import QApplication
    import numpy as np
    
    app = QApplication(sys.argv)
    d2 = Measurements_Data(np.zeros((1,4)), columns = ['time', 'U', 'I', 'Iout'])
    d2.changed.connect(hello2)
    d2.Data = pd.concat([d2.Data,
                         pd.DataFrame(np.ones((2,4)),columns = d2.Data.columns)],
                        axis = 0, ignore_index = True)
  
